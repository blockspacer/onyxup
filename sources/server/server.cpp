#include "server.h"

using namespace std::chrono_literals;
using json = nlohmann::json;

#ifdef DEBUG_MODE
static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
#else
static plog::ColorConsoleAppender<onyxup::Formatter<>> consoleAppender;
#endif

bool onyxup::HttpServer::isStatisticsEnable = false;
std::string onyxup::HttpServer::statisticsUrl("^/onyxup-status-page$");
std::string onyxup::HttpServer::pathToStaticResources;
int onyxup::HttpServer::timeLimitRequestSeconds = 60;
int onyxup::HttpServer::limitLocalTasks = 100;
bool onyxup::HttpServer::isCompressStaticResources = false;
bool onyxup::HttpServer::isCachedStaticResources = true;
std::string onyxup::HttpServer::pathToConfigurationFile;
std::unordered_map<std::string, std::string> onyxup::HttpServer::mimeTypesMap;
std::unordered_map<std::string, onyxup::ResponseBase> onyxup::HttpServer::cachedStaticResourcesMap;

std::unique_ptr<onyxup::StatisticsService> statisticsService(nullptr);

static json parseConfigurationFile(const std::string &filename) {
    json settings;
    try {
        std::string buffer;
        std::ifstream file(filename, std::ios::in | std::ifstream::binary);
        if (file.good()) {
            buffer = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            settings = json::parse(buffer);
        }
    } catch (json::exception &ex) {
        LOGE << "Конфигурационный файл " << filename << " ошибка чтения";
    }
    return settings;
}

static int setNonBlockingModeSocket(int fd) {
    int flags;
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int onyxup::HttpServer::writeToOutputBuffer(int fd, const char *data, size_t len) noexcept {
    PtrBuffer buffer = buffers[fd];
    int code = ResponseState::RESPONSE_STATE_OK_CODE;
    if (buffer->getPosOutputBuffer() + len >= maxOutputBufferLength) {
        buffer->clear();
        Response413 response = Response413();
        response.addHeader("Content-Length",
                           std::to_string(response.getBody().size()));
        std::string str = response.toString();
        buffer->addDataToOutputBuffer(str.c_str(), str.size());
        LOGD << "Превышен размер выходного буфера";
        code = ResponseState::RESPONSE_STATE_PAYLOAD_TOO_LARGE_CODE;
    } else
        buffer->addDataToOutputBuffer(data, len);
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP;

    if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event) == -1) {
        LOGE << "Не возможно модифицировать файловый дескриптор. Ошибка " << errno;
        closeAllSocketsAndClearData(fd);
        return -1;
    }
    return code;
}

onyxup::PtrTask onyxup::HttpServer::dispatcher(PtrRequest request) noexcept {
    PtrRequest req = req::requestCopyFactory(request);
    if (req) {
        PtrTask task = taskFactory();
        if (task) {
            std::vector<Route>::iterator it = routes.begin();
            while (it != routes.end()) {
                if (it->getMethod() == req->getMethod()) {
                    regmatch_t pm;
                    regex_t regex = it->getPregex();
                    if (regexec(&regex, req->getFullURIRef().c_str(), 0, &pm, 0) == 0) {
                        task->setType(it->getTaskType());
                        task->setRequest(req);
                        task->setHandler(it->getHandler());
                        return task;
                    }
                }
                ++it;
            }
        }
        delete req;
        delete task;
        return nullptr;
    }
    LOGE << "Не возможно создать Onyxup Request. Ошибка выделения памяти";
    return nullptr;
}

void onyxup::HttpServer::addRoute(const std::string &method, const char *regex,
                                  std::function<ResponseBase(PtrCRequest request)> handler,
                                  EnumTaskType task_type) noexcept {
    std::string methodToUpperCase(method);
    std::transform(methodToUpperCase.begin(), methodToUpperCase.end(), methodToUpperCase.begin(),
                   [](unsigned char c) {
        return std::toupper(c);
    });
    routes.push_back(Route(methodToUpperCase, regex, handler, task_type));
    if (methodToUpperCase == "GET")
        routes.push_back(Route("HEAD", regex, handler, task_type));
}

void onyxup::HttpServer::tasksHandler(int id) {
    
    thread_local std::shared_ptr<ResponsePrepareHeadChain> responsePrepareHeadChain (new ResponsePrepareHeadChain);
    thread_local std::shared_ptr<ResponsePrepareRangeChain> responsePrepareRangeChain (new ResponsePrepareRangeChain);
    thread_local std::shared_ptr<ResponsePrepareCompressChain> responsePrepareCompressChain(new ResponsePrepareCompressChain(isCompressStaticResources));
    thread_local std::shared_ptr<ResponsePrepareDefaultChain> responsePrepareDefaultChain (new ResponsePrepareDefaultChain);
    
    responsePrepareHeadChain->setNextHandler(responsePrepareRangeChain);
    responsePrepareRangeChain->setNextHandler(responsePrepareCompressChain);
    responsePrepareCompressChain->setNextHandler(responsePrepareDefaultChain);
    
    while (true) {
        PtrTask task = nullptr;
        if (id)
            tasksQueue.wait_and_pop(task);
        else
            staticTasksQueue.wait_and_pop(task);
        if (task->getType() == EnumTaskType::LOCAL_TASK || task->getType() == EnumTaskType::STATIC_RESOURCES_TASK) {
            ResponseBase response = task->getHandler()(task->getRequest());
            task->setCode(response.getCode());
            /*
             * Запускаем цепочку обработчиков
             */
            responsePrepareHeadChain->execute(task, response);
            task->setResponseData(response);
        }
        performedTasksQueue.push(task);
    }
}

onyxup::HttpServer::HttpServer(int port, size_t n) : numberThreads(n) {

#ifdef DEBUG_MODE
    plog::init(plog::debug, &consoleAppender);
#else
    plog::init(plog::info, &consoleAppender);
#endif

    pathToConfigurationFile = "/var/onyxup/config.json";
    auto settings = parseConfigurationFile(pathToConfigurationFile);
    
    if (settings.find("server") != settings.end()) {
        json json_server = settings["server"];
        try {
            if (json_server.find("threads") != json_server.end())
                numberThreads = settings["server"]["threads"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> threads должно быть целым";
        }
        try {
            if (json_server.find("max_connection") != json_server.end())
                maxConnection = settings["server"]["max_connection"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> max_connection должно быть целым";
        }
        try {
            if (json_server.find("max_input_length_buffer") != json_server.end())
                maxInputBufferLength = settings["server"]["max_input_length_buffer"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> max_input_length_buffer должно быть целым";
        }
        try {
            if (json_server.find("max_output_length_buffer") != json_server.end())
                maxOutputBufferLength = settings["server"]["max_output_length_buffer"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> max_output_length_buffer должно быть целым";
        }
        try {
            if (json_server.find("time_limit_request_seconds") != json_server.end())
                timeLimitRequestSeconds = settings["server"]["time_limit_request_seconds"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> time_limit_request_seconds должно быть целым";
        }
        try {
            if (json_server.find("limit_local_tasks") != json_server.end())
                limitLocalTasks = settings["server"]["limit_local_tasks"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> limit_local_tasks должно быть целым";
        }
    }
    if (settings.find("static-resources") != settings.end()) {
        json json_static_resources = settings["static-resources"];
        try {
            if (json_static_resources.find("directory") != json_static_resources.end())
                pathToStaticResources = settings["static-resources"]["directory"].get<std::string>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле static-resources -> directory должно быть строковым";
        }
        try {
            if (json_static_resources.find("compress") != json_static_resources.end())
                isCompressStaticResources = settings["static-resources"]["compress"].get<bool>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле static-resources -> compress должно быть булевым";
        }
        try {
            if (json_static_resources.find("cache") != json_static_resources.end())
                isCachedStaticResources = settings["static-resources"]["cache"].get<bool>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле static-resources -> cache должно быть булевым";
        }
    }
    if (settings.find("statistics") != settings.end()) {
        try {
            isStatisticsEnable = settings["enable"].get<bool>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле statistics -> enable должно быть булевым";
        }
        try {
            statisticsUrl = settings["url"].get<std::string>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле statistics -> url должно быть строковым";
        }
    }

    buffers = new PtrBuffer[maxConnection];
    requests = new PtrRequest[maxConnection];
    statisticsService.reset(new StatisticsService(buffers, requests, maxConnection));
    aliveSockets.resize(maxConnection);
    for (size_t i = 0; i < maxConnection; i++) {
        buffers[i] = nullptr;
        requests[i] = nullptr;
        aliveSockets[i] = std::chrono::steady_clock::now();
    }

    mimeTypesMap = MimeType::generateMimeTypesMap();
    if (numberThreads < 2)
        numberThreads = 2;

    threadsPool.resize(numberThreads);

    for (size_t i = 0; i < numberThreads; i++) {
        std::thread t(&HttpServer::tasksHandler, this, i);
        threadsPool[i] = std::move(t);
    }

    struct sockaddr_in server_addr;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        LOGE << "Не возможно создать серверный сокет";
        throw OnyxupException("Не возможно создать серверный сокет");
    }
    int enable = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        close(fd);
        LOGE << "Не возможно создать серверный сокет. Ошибка " << errno;
        throw OnyxupException("Не возможно создать серверный сокет");
    }
    if (setNonBlockingModeSocket(fd) == -1) {
        LOGE << "Не возможно создать серверный сокет. Ошибка " << errno;
        throw OnyxupException("Не возможно создать серверный сокет");
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    server_addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        close(fd);
        LOGE << "Не возможно выполнить операцию bind. Ошибка " << errno;
        throw OnyxupException("Не возможно создать серверный сокет");
    }

    if (listen(fd, SOMAXCONN) < 0) {
        LOGE << "Не возможно выполнить операцию listen. Ошибка " << errno;
        throw OnyxupException("Не возможно создать серверный сокет");
    };

    epollFd = epoll_create1(0);
    if (epollFd == -1) {
        close(fd);
        LOGE << "Не возможно создать epoll. Ошибка " << errno;
        throw OnyxupException("Не возможно создать серверный сокет");
    }

    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;

    epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
    
    ResponseBase::SERVER_PORT = port;
    ResponseBase::SERVER_IP = std::string(inet_ntoa(server_addr.sin_addr));
}

void onyxup::HttpServer::run() noexcept {
    sockaddr_in peer_addr;
    int address_length = sizeof(peer_addr);
    struct epoll_event events[maxEventsEpoll];
    struct epoll_event event;

    /*
     * Подключение сбора статистики
     */
    if (isStatisticsEnable) {
        this->addRoute("GET", statisticsUrl.c_str(), [](PtrCRequest request) -> ResponseBase {
            return statisticsService->callback(request);
        }, EnumTaskType::LOCAL_TASK);
    }

    for (;;) {
        static size_t counter_check_limit_time_request = 0;
        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

        if (isStatisticsEnable)
            statisticsService->setCurrentNumberTasks(tasksQueue.size() + staticTasksQueue.size());
        for (size_t i = counter_check_limit_time_request;
             i < maxConnection && i < counter_check_limit_time_request + 100; i++) {
            if (requests[i]) {
                if (std::chrono::duration_cast<std::chrono::seconds>(now - aliveSockets[i]).count() >
                    HttpServer::timeLimitRequestSeconds) {
                    if (requests[i]->getFullURIRef().empty())
                        closeAllSocketsAndClearData(requests[i]->getFD());
                    else {
                        ResponseBase response = std::move(onyxup::Response408());
                        response.addHeader("Content-Length", std::to_string(response.getBody().size()));
                        std::string str = response.toString();
                        writeToOutputBuffer(requests[i]->getFD(), str.c_str(), str.length());
                        requests[i]->setClosingConnect(true);
                        aliveSockets[requests[i]->getFD()] = std::chrono::steady_clock::now();
                        LOGI << requests[i]->getMethod() << " " << requests[i]->getFullURIRef() << " "
                             << ResponseState::RESPONSE_STATE_METHOD_REQUEST_TIMEOUT_CODE;
                    }
                }
            }
        }
        counter_check_limit_time_request = (counter_check_limit_time_request + 100) % maxConnection;

        while (!performedTasksQueue.empty()) {
            PtrTask task = nullptr;
            if (performedTasksQueue.try_pop(task)) {
                /*
                 * Проверяем что данный сокет еще жив
                */
                if (task->getTimePoint() == aliveSockets[task->getFD()]) {
                    int code = writeToOutputBuffer(task->getFD(), task->getResponseData().c_str(),
                                                   task->getResponseData().size());
                    if (code == ResponseState::RESPONSE_STATE_PAYLOAD_TOO_LARGE_CODE)
                        LOGI << task->getRequest()->getMethod() << " " << task->getRequest()->getFullURIRef() << " "
                             << ResponseState::RESPONSE_STATE_PAYLOAD_TOO_LARGE_CODE;
                    else LOGI << task->getRequest()->getMethod() << " " << task->getRequest()->getFullURIRef() << " "
                                  << task->getCode();
                }
                delete task;
            }
        }
        int fds = epoll_wait(epollFd, events, maxEventsEpoll, 100);
        for (int i = 0; i < fds; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (events[i].events & EPOLLRDHUP)) {
                closeAllSocketsAndClearData(events[i].data.fd);
                continue;
            }
            if (events[i].data.fd == fd) {
                int conn_sock = accept(fd, (struct sockaddr *) &peer_addr, (socklen_t *) &address_length);
                if (conn_sock > (int) maxConnection - 1) {
                    closeSocket(conn_sock);
                    LOGE << "Превышено максимальное количество соединений на сервере";
                    continue;
                }
                if (conn_sock == -1) {
                    closeSocket(conn_sock);
                    LOGE << "Не возможно принять соединение на сервере. Ошибка " << errno;
                    continue;
                }
                if (setNonBlockingModeSocket(conn_sock) == -1) {
                    closeSocket(conn_sock);
                    LOGE << "Не возможно перевести сокет в неблокирующий режим. Ошибка " << errno;
                    continue;
                }
                event.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
                event.data.fd = conn_sock;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, conn_sock, &event) == -1) {
                    closeSocket(conn_sock);
                    LOGE << "Не возможно добавить файловый дескриптор в epoll. Ошибка " << errno;
                    continue;
                }

                if (isStatisticsEnable)
                    statisticsService->addTotalNumberConnectionsAccepted();

                aliveSockets[conn_sock] = std::chrono::steady_clock::now();

                /*
                 * Подготавливаем входной и выходной буфер, выделяем память
                 */
                buffers[conn_sock] = buffer::bufferFactory(maxInputBufferLength, maxOutputBufferLength);
                if (buffers[conn_sock] == nullptr) {
                    LOGE << "Ошибка выделение памяти";
                    closeSocket(conn_sock);
                    continue;
                }

                /*
                 * Подготавливаем req
                 */

                requests[conn_sock] = req::requestFactory();
                if (requests[conn_sock] == nullptr) {
                    closeSocket(conn_sock);
                    delete buffers[conn_sock];
                    buffers[conn_sock] = nullptr;
                    continue;
                }
                requests[conn_sock]->setFD(conn_sock);
                requests[conn_sock]->setMaxOutputLengthBuffer(maxOutputBufferLength);
            } else {
                if (events[i].events & EPOLLIN) {
                    char data[4096];
                    int res = recv(events[i].data.fd, data, sizeof(data), 0);
                    if (res == -1) {
                        if (errno != EWOULDBLOCK && errno != EINPROGRESS) {
                            LOGE << "Не возможно прочитать данные из сокета. Ошибка " << errno;
                            closeAllSocketsAndClearData(events[i].data.fd);
                            continue;
                        }
                    }
                    if (res == 0) {
                        closeAllSocketsAndClearData(events[i].data.fd);
                        continue;
                    }
                    PtrBuffer buffer = buffers[events[i].data.fd];
                    if (buffer->getPosInputBuffer() + res >= maxInputBufferLength) {
                        LOGD << "Превышен размер входного буфера";

                        Response413 response = Response413();
                        response.addHeader("Content-Length",
                                           std::to_string(response.getBody().size()));
                        std::string str = response.toString();
                        writeToOutputBuffer(events[i].data.fd, str.c_str(), str.length());
                        requests[events[i].data.fd]->setClosingConnect(true);
                        requests[events[i].data.fd]->setHeaderAccept(true);
                        continue;
                    }
                    buffer->addDataToInputBuffer(data, res);
                    /*
                     * Парсим http запрос
                     */

                    const char *method, *uri;
                    phr_header headers[100];
                    size_t method_len, uri_len;
                    int version;
                    size_t num_headers = sizeof(headers) / sizeof(headers[0]);
                    int parse_http_result = phr_parse_request(buffer->getInputBuffer(),
                                                              buffer->getPosInputBuffer() + 1,
                                            &method,
                                            &method_len, &uri, &uri_len, &version, headers,
                                            &num_headers, 0);
                    if (parse_http_result > 0) {
                        PtrRequest request = requests[events[i].data.fd];

                        if (!request->isHeaderAccept()) {
                            request->setHeaderAccept(true);
                            request->setFullURI(uri, uri_len);
                            request->setMethod(method, method_len);

                            for (size_t j = 0; j < num_headers; j++) {
                                std::string key(headers[j].name, (int) headers[j].name_len);
                                std::transform(key.begin(), key.end(), key.begin(), tolower);
                                std::string value(headers[j].value, (int) headers[j].value_len);
                                request->addHeader(key, value);
                            }
                            /*
                             * Получаем параметры строки запроса
                             */
                            utils::parseParamsRequest(request, uri_len);

                            /*
                             * Определяем должен ли запрос содержать тело
                             */
                            try {
                                std::string header_content_length = request->getHeaderRef("content-length");
                                int len = atoi(header_content_length.c_str());
                                if (len)
                                    request->setBodyExists(true);
                            } catch (std::out_of_range &ex) {
                            }
                        }

                        /*
                         * Получаем тело запроса
                         */
                        if (request->isBodyExists()) {
                            try {
                                std::string header_content_length = request->getHeaderRef("content-length");
                                int len = atoi(header_content_length.c_str());
                                if (len && buffer->getPosInputBuffer() == (parse_http_result + len)) {
                                    request->setBodyAccept(true);
                                    request->setBody(buffer->getInputBuffer() + parse_http_result, len);
                                }
                            } catch (std::out_of_range &ex) {
                                closeAllSocketsAndClearData(events[i].data.fd);
                                continue;
                            }
                            if (!request->isBodyAccept())
                                continue;
                        }

                        if ((request->isBodyExists() && request->isBodyAccept()) || !request->isBodyExists()) {
                            struct epoll_event event;
                            event.data.fd = events[i].data.fd;
                            event.events = EPOLLERR | EPOLLHUP | EPOLLRDHUP;
                            if (epoll_ctl(epollFd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1) {
                                LOGE << "Не возможно модифицировать файловый дескриптор в epoll. Ошибка " << errno;
                                closeAllSocketsAndClearData(events[i].data.fd);
                                continue;
                            }
                        }

                        /*
                         * Запускаем dispatcher
                         */
                        PtrTask task = dispatcher(request);
                        if (task) {
                            task->setFD(events[i].data.fd);
                            task->setTimePoint(aliveSockets[events[i].data.fd]);
                            /*
                             * В зависимости от типа задачи направляем в соответствующий поток
                             */
                            if (task->getType() == EnumTaskType::LOCAL_TASK) {
                                if (tasksQueue.size() > HttpServer::limitLocalTasks) {
                                    ResponseBase response = onyxup::Response503();
                                    response.addHeader("Content-Length",
                                                       std::to_string(response.getBody().size()));
                                    std::string str = response.toString();
                                    writeToOutputBuffer(events[i].data.fd, str.c_str(), str.size());
                                    LOGI << request->getMethod() << " " << request->getFullURIRef() << " "
                                         << ResponseState::RESPONSE_STATE_SERVICE_UNAVAILABLE_CODE;
                                    delete task;
                                } else
                                    addTask(task);
                            } else if (task->getType() == EnumTaskType::STATIC_RESOURCES_TASK) {
                                addTask(task);
                            } else {
                                LOGE << "Не известный тип задачи";
                                delete task;
                                closeAllSocketsAndClearData(events[i].data.fd);
                            }
                            statisticsService->addTotalNumberClientRequests();
                        } else {
                            ResponseBase response = onyxup::Response404();
                            response.addHeader("Content-Length", std::to_string(response.getBody().size()));
                            std::string str = response.toString();
                            writeToOutputBuffer(events[i].data.fd, str.c_str(), str.size());
                            LOGI << request->getMethod() << " " << request->getFullURIRef() << " "
                                 << ResponseState::RESPONSE_STATE_NOT_FOUND_CODE;
                            statisticsService->addTotalNumberClientRequests();
                        }
                        continue;
                    } else if (parse_http_result == -2) {
                        continue;
                    } else if (parse_http_result == -1) {
                        LOGE << "Ошибка разбора HTTP запроса";
                        closeAllSocketsAndClearData(events[i].data.fd);
                        continue;
                    }
                }
                if (events[i].events & EPOLLOUT && buffers[events[i].data.fd]->getBytesToSend() > 0 &&
                    requests[events[i].data.fd]->isHeaderAccept()) {
                    if (requests[events[i].data.fd]->isBodyExists() &&
                        !requests[events[i].data.fd]->isBodyAccept())
                        continue;
                    PtrBuffer buffer = buffers[events[i].data.fd];
                    int res = send(events[i].data.fd, buffer->getOutputBuffer() + buffer->getPosOutputBuffer(),
                                   buffer->getBytesToSend(), 0);
                    if (res == -1) {
                        closeAllSocketsAndClearData(events[i].data.fd);
                        continue;
                    }
                    buffer->setBytesToSend(buffer->getBytesToSend() - res);
                    buffer->setPosOutputBuffer(buffer->getPosOutputBuffer() + res);
                    if (buffer->getBytesToSend() == 0) {
                        try {
                            if (requests[events[i].data.fd]->isClosingConnect()) {
                                closeAllSocketsAndClearData(events[i].data.fd);
                            } else if (requests[events[i].data.fd]->getHeader("connection") == "Keep-Alive") {
                                buffers[events[i].data.fd]->clear();
                                requests[events[i].data.fd]->clear();
                                struct epoll_event event;
                                event.data.fd = events[i].data.fd;
                                event.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
                                if (epoll_ctl(epollFd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1) {
                                    closeAllSocketsAndClearData(events[i].data.fd);
                                    LOGE << "Не возможно модифицировать файловый дескриптор в epoll. Ошибка " << errno;
                                    continue;
                                }
                                aliveSockets[events[i].data.fd] = std::chrono::steady_clock::now();
                            } else
                                closeAllSocketsAndClearData(events[i].data.fd);
                        } catch (std::out_of_range &ex) {
                            closeAllSocketsAndClearData(events[i].data.fd);
                        }
                    }
                }
            }
        }
    }
}

onyxup::HttpServer::~HttpServer() {
    for (size_t i = 0; i < numberThreads; i++)
        threadsPool[i].join();

    for (size_t i = 0; i < maxConnection; i++)
        closeAllSocketsAndClearData(i);
    shutdown(fd, SHUT_RD);
    close(fd);
    delete[] buffers;
    delete[] requests;
}

onyxup::ResponseBase onyxup::HttpServer::defaultStaticResourcesCallback(onyxup::PtrCRequest request) {
    /*
     * Определяем content type по расширению файла
     */
    char content_type_key[10] = {'\0'};
    char *token_dot = strrchr((char *) request->getFullURIRef().c_str(), '.');
    if (token_dot == NULL)
        return onyxup::Response404();
    if (strlen(token_dot + 1) > sizeof(content_type_key) - 1) {
        return onyxup::Response404();
    }
    strcat(content_type_key, token_dot + 1);
    std::unordered_map<std::string, std::string>::iterator it = mimeTypesMap.find(
                content_type_key);
    if (it == mimeTypesMap.end()) {
        return onyxup::Response404();
    }

    /*
     * Проверяем кеш иначе получаем ресурс и заносим в кеш
     */
    if (isCachedStaticResources) {
        std::unordered_map<std::string, ResponseBase>::iterator iter = cachedStaticResourcesMap.find(
                    request->getURIRef());
        if (iter == cachedStaticResourcesMap.end()) {
            char path_to_file[120];
            snprintf(path_to_file, sizeof(path_to_file), "%s%s",
                     pathToStaticResources.c_str(), request->getURIRef().c_str());

            std::ifstream file(path_to_file, std::ios::in | std::ifstream::binary);
            if (file.good()) {
                std::string buf = std::string((std::istreambuf_iterator<char>(file)),
                                              std::istreambuf_iterator<char>());
                file.close();
                ResponseBase response(ResponseState::RESPONSE_STATE_OK_CODE, ResponseState::RESPONSE_STATE_OK_MSG,
                                      it->second.c_str(), buf);
                /*
                 * Отправляем данные в кеш
                 */
                cachedStaticResourcesMap[request->getURIRef()] = response;
                return response;
            } else {
                return onyxup::Response404();
            }
        } else {
            /*
             * Берем из кеша
             */
            ResponseBase response = cachedStaticResourcesMap[request->getURIRef()];
            return response;
        }
    } else {
        char path_to_file[120];
        snprintf(path_to_file, sizeof(path_to_file), "%s%s",
                 pathToStaticResources.c_str(), request->getURIRef().c_str());

        std::ifstream file(path_to_file, std::ios::in | std::ifstream::binary);
        if (file.good()) {
            std::string buf = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            ResponseBase response(ResponseState::RESPONSE_STATE_OK_CODE, ResponseState::RESPONSE_STATE_OK_MSG,
                                  it->second.c_str(), buf);
            /*
             * Отправляем данные в кеш
             */
            cachedStaticResourcesMap[request->getURIRef()] = response;
            return response;
        } else {
            return onyxup::Response404();
        }
    }

}

int onyxup::HttpServer::getTimeLimitRequestSeconds() {
    return timeLimitRequestSeconds;
}

void onyxup::HttpServer::setTimeLimitRequestSeconds(int limit) {
    timeLimitRequestSeconds = limit;
}

int onyxup::HttpServer::getLimitLocalTasks() {
    return limitLocalTasks;
}

void onyxup::HttpServer::setLimitLocalTasks(int limit) {
    limitLocalTasks = limit;
}

void onyxup::HttpServer::setCachedStaticResources(bool flag) {
    isCachedStaticResources = flag;
}

void onyxup::HttpServer::setPathToConfigurationFile(const std::string &file) {
    pathToConfigurationFile = file;
}

void onyxup::HttpServer::setStatisticsEnable(bool enable) {
    isStatisticsEnable = enable;
}

void onyxup::HttpServer::setStatisticsUrl(const std::string &url) {
    statisticsUrl = url;
}

void onyxup::HttpServer::closeAllSocketsAndClearData(int fd) {
    shutdown(fd, SHUT_RDWR);
    close(fd);
    delete buffers[fd];
    delete requests[fd];
    aliveSockets[fd] = std::chrono::steady_clock::now();
    buffers[fd] = nullptr;
    requests[fd] = nullptr;
    statisticsService->addTotalNumberConnectionsProcessed();
}

void onyxup::HttpServer::setMaxInputBufferLength(size_t len) {
    maxInputBufferLength = len;
}

void onyxup::HttpServer::setMaxOutputBufferLength(size_t len) {
    maxOutputBufferLength = len;
}
