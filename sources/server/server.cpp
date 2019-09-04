#include "server.h"
#include "../services/statistics/StatisticsService.h"

using namespace std::chrono_literals;
using json = nlohmann::json;

#ifdef DEBUG_MODE
static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
#else
static plog::ColorConsoleAppender<onyxup::Formatter<>> consoleAppender;
#endif

bool onyxup::HttpServer::m_statistics_enable = false;
std::string onyxup::HttpServer::m_statistics_url("^/onyxup-status-page$");
std::string onyxup::HttpServer::m_path_to_static_resources;
int onyxup::HttpServer::m_time_limit_request_seconds = 60;
int onyxup::HttpServer::m_limit_local_tasks = 100;
bool onyxup::HttpServer::m_compress_static_resources = false;
bool onyxup::HttpServer::m_cached_static_resources = false;
std::string onyxup::HttpServer::m_path_to_configuration_file;
std::unordered_map<std::string, std::string> onyxup::HttpServer::m_map_mime_types;
std::unordered_map<std::string, onyxup::ResponseBase> onyxup::HttpServer::m_map_cached_static_resources;

std::unique_ptr<onyxup::StatisticsService> statistics_service(nullptr);

static std::string trim_spaces(const std::string &src) {
    std::string dst;
    for (size_t i = 0; i < src.size(); i++)
        if (src[i] != ' ')
            dst += src[i];
    return dst;
}

static json parse_configuration_file(const std::string &filename) {
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

static std::string trim_characters(const std::vector<char> &src, std::initializer_list<char> chars) {
    auto start = src.begin(), end = src.end() - 1;
    while (start != end) {
        bool exists = false;
        for (auto &ch : chars) {
            if (*start == ch) {
                exists = true;
                break;
            }
        }
        if (exists)
            start++;
        else
            break;
    }
    while (end != start) {
        bool exists = false;
        for (auto &ch : chars) {
            if (*end == ch) {
                exists = true;
                break;
            }
        }
        if (exists)
            end--;
        else
            break;
    }
    return std::string(start, end + 1);
}

static int set_non_blocking_mode_socket(int fd) {
    int flags;
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static bool check_client_support_gzip_encoding(onyxup::PtrTask task) {
    try {
        if (task->getRequest()->getHeaderRef("accept-encoding").find("gzip") != std::string::npos)
            return true;
    } catch (std::out_of_range &ex) {
        return false;
    }
}

static bool check_client_request_range(onyxup::PtrTask task) {
    try {
        if (task->getRequest()->getHeaderRef("range").find("bytes") != std::string::npos)
            return true;
    } catch (std::out_of_range &ex) {
        return false;
    }
}

void static prepare_range_not_satisfiable_response(onyxup::ResponseBase &response) {
    std::ostringstream os;
    os << "*/" << response.getBody().size();
    response.setCode(onyxup::ResponseState::RESPONSE_STATE_RANGE_NOT_SATISFIABLE_CODE);
    response.setCodeMsg(onyxup::ResponseState::RESPONSE_STATE_RANGE_NOT_SATISFIABLE_MSG);
    response.appendHeader("Content-Range", os.str());
    response.setBody("");
}

static void prepare_range_response(onyxup::ResponseBase &response, std::vector<std::pair<size_t, size_t>> &ranges) {
    if (ranges.size() == 1) {
        std::string body;
        std::copy(response.getBody().begin() + ranges[0].first,
                  response.getBody().begin() + ranges[0].second + 1,
                  std::back_insert_iterator<std::string>(body));
        std::ostringstream os;
        os << "bytes " << ranges[0].first << "-" << ranges[0].second << "/"
           << response.getBody().size();
        response.setCode(onyxup::ResponseState::RESPONSE_STATE_PARTIAL_CONTENT_CODE);
        response.setCodeMsg(onyxup::ResponseState::RESPONSE_STATE_PARTIAL_CONTENT_MSG);
        response.appendHeader("Content-Range", os.str());
        response.appendHeader("Content-Length", std::to_string(body.size()));
        response.setBody(body);
    } else {
        size_t length_body = response.getBody().size();
        const char *content_type_body = response.getMime();
        std::ostringstream os;
        response.setCode(onyxup::ResponseState::RESPONSE_STATE_PARTIAL_CONTENT_CODE);
        response.setCodeMsg(onyxup::ResponseState::RESPONSE_STATE_PARTIAL_CONTENT_MSG);
        response.setMime(onyxup::MimeType::MIME_TYPE_MULTIPART_BYTES_RANGES);
        for (size_t i = 0; i < ranges.size(); i++) {
            auto range = ranges[i];
            std::string body;
            std::copy(response.getBody().begin() + range.first,
                      response.getBody().begin() + range.second + 1,
                      std::back_insert_iterator<std::string>(body));
            os << "--3d6b6a416f9b5\r\n" << "Content-Type: " << content_type_body << "\r\n"
               << "Content-Range: bytes " << range.first << "-" << range.second << "/"
               << length_body << "\r\n\r\n" << body;
            if (i < ranges.size() - 1)
                os << "\r\n";
        }
        response.setBody(os.str());
        response.appendHeader("Content-Length", std::to_string(response.getBody().size()));
    }
}

static void prepare_head_response(onyxup::ResponseBase &response) {
    response.appendHeader("Content-Length", std::to_string(response.getBody().size()));
    response.setBody("");
}

static void prepare_default_response(onyxup::ResponseBase &response) {
    response.appendHeader("Content-Length", std::to_string(response.getBody().size()));
}

static void prepare_compress_response(onyxup::ResponseBase &response) {
    response.appendHeader("Content-Encoding", "gzip");
    std::string compressed_body = gzip::compress(response.getBody().c_str(), response.getBody().size());
    response.setBody(compressed_body);
    response.appendHeader("Content-Length", std::to_string(response.getBody().size()));
}

int onyxup::HttpServer::writeToOutputBuffer(int fd, const char *data, size_t len) noexcept {
    PtrBuffer buffer = m_buffers[fd];
    int code = ResponseState::RESPONSE_STATE_OK_CODE;
    if (buffer->getOutBufferPosition() + len >= m_max_output_length_buffer) {
        buffer->clear();
        Response413 response = Response413();
        response.appendHeader("Content-Length",
                              std::to_string(response.getBody().size()));
        std::string str = response.to_string();
        buffer->appendOutBuffer(str.c_str(), str.size());
        LOGD << "Превышен размер выходного буфера";
        code = ResponseState::RESPONSE_STATE_PAYLOAD_TOO_LARGE_CODE;
    } else
        buffer->appendOutBuffer(data, len);
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLOUT | EPOLLERR | EPOLLHUP | EPOLLRDHUP;

    if (epoll_ctl(m_e_poll_fd, EPOLL_CTL_MOD, fd, &event) == -1) {
        LOGE << "Не возможно модифицировать файловый дескриптор. Ошибка " << errno;
        closeAllSocketsAndClearData(fd);
        return -1;
    }
    return code;
}

void onyxup::HttpServer::parseParamsRequest(onyxup::PtrRequest request, size_t uri_len) noexcept {
    const char *full_uri = request->getFullURI().c_str();
    char *start_params_part = const_cast<char *>(strchr(full_uri, '?'));
    if (start_params_part) {
        request->setURI(std::string(full_uri, start_params_part - full_uri));
        size_t buf_params_len = uri_len - (start_params_part - full_uri);
        char *buf_params = new(std::nothrow) char[buf_params_len];
        if (buf_params) {
            memcpy(buf_params, start_params_part + 1, buf_params_len);
            char *save_ptr;
            char *token = strtok_r(buf_params, "&", &save_ptr);
            while (token != NULL) {
                char *sep = strchr(token, '=');
                if (sep) {
                    std::string key(token, sep - token);
                    std::string value(sep + 1, strlen(token) - (sep - token) - 1);
                    request->appendParam(key, value);
                }
                token = strtok_r(NULL, "&", &save_ptr);
            }
            delete[] buf_params;
        }
    } else
        request->setURI(request->getFullURI());
}

std::vector<std::pair<size_t, size_t>>
onyxup::HttpServer::parseRangesRequest(const std::string &src, size_t max_index_byte) {
    std::vector<std::pair<size_t, size_t>> ranges;
    if (!max_index_byte)
        throw onyxup::OnyxupException("Неверный формат HTTP range requests");

    int pos = src.find("bytes=");
    if (pos != std::string::npos) {
        std::istringstream iss(src.substr(pos + strlen("bytes=")));
        std::string token;
        while (std::getline(iss, token, ',')) {
            std::string token_with_out_spaces(std::move(trim_spaces(token)));
            if (token_with_out_spaces[token_with_out_spaces.size() - 1] == '-') {
                size_t value = 0;
                for (size_t i = 0; i < token_with_out_spaces.size() - 1; i++) {
                    if (!std::isdigit(token_with_out_spaces[i]))
                        throw onyxup::OnyxupException("Неверный формат HTTP range requests");
                    value = value * 10 + (token_with_out_spaces[i] - '0');
                }
                if (value > max_index_byte)
                    throw onyxup::OnyxupException("Неверный формат HTTP range requests");
                ranges.emplace_back(value, max_index_byte);
            } else if (token_with_out_spaces[0] == '-') {
                size_t value = 0;
                for (size_t i = 1; i < token_with_out_spaces.size(); i++) {
                    if (!std::isdigit(token_with_out_spaces[i]))
                        throw onyxup::OnyxupException("Неверный формат HTTP range requests");
                    value = value * 10 + (token_with_out_spaces[i] - '0');
                }
                if (value > max_index_byte)
                    throw onyxup::OnyxupException("Неверный формат HTTP range requests");
                ranges.emplace_back(max_index_byte - value, max_index_byte);
            } else {
                size_t pos = token_with_out_spaces.find("-");
                if (pos == std::string::npos)
                    throw onyxup::OnyxupException("Неверный формат HTTP range requests");

                size_t value_left = 0;
                for (size_t i = 0; i < pos; i++) {
                    if (!std::isdigit(token_with_out_spaces[i]))
                        throw onyxup::OnyxupException("Неверный формат HTTP range requests");
                    value_left = value_left * 10 + (token_with_out_spaces[i] - '0');
                }
                size_t value_right = 0;
                for (size_t i = pos + 1; i < token_with_out_spaces.size(); i++) {
                    if (!std::isdigit(token_with_out_spaces[i]))
                        throw onyxup::OnyxupException("Неверный формат HTTP range requests");
                    value_right = value_right * 10 + (token_with_out_spaces[i] - '0');
                }
                if (value_right < value_left || value_right > max_index_byte)
                    throw onyxup::OnyxupException("Неверный формат HTTP range requests");
                ranges.emplace_back(value_left, value_right);
            }
        }
        if (ranges.empty())
            throw onyxup::OnyxupException("Неверный формат HTTP range requests");
        std::sort(ranges.begin(), ranges.end(), [](auto &first, auto &second) -> bool {
            return first.first < second.first;
        });
        ssize_t border = 0;
        for (size_t i = 0; i < ranges.size(); i++) {
            std::pair<size_t, size_t> &pair = ranges[i];
            if (i) {
                if (pair.first < border || pair.second > max_index_byte)
                    throw onyxup::OnyxupException("Неверный формат HTTP range requests");
            }
            border = pair.second;
        }
        return ranges;
    }
    throw onyxup::OnyxupException("Неверный формат HTTP range requests");
}

onyxup::PtrTask onyxup::HttpServer::dispatcher(PtrRequest request) noexcept {
    PtrRequest req = request::factoryCopyRequest(request);
    if (req) {
        PtrTask task = factoryTask();
        if (task) {
            std::vector<Route>::iterator it = m_routes.begin();
            while (it != m_routes.end()) {
                if (it->getMethod() == req->getMethod()) {
                    regmatch_t pm;
                    regex_t regex = it->getPregex();
                    if (regexec(&regex, req->getFullURI().c_str(), 0, &pm, 0) == 0) {
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
    std::string method_to_upper_case(method);
    std::transform(method_to_upper_case.begin(), method_to_upper_case.end(), method_to_upper_case.begin(),
                   [](unsigned char c) {
                       return std::toupper(c);
                   });
    m_routes.push_back(Route(method_to_upper_case, regex, handler, task_type));
    if (method_to_upper_case == "GET")
        m_routes.push_back(Route("HEAD", regex, handler, task_type));
}

void onyxup::HttpServer::handler_tasks(int id) {
    while (true) {
        PtrTask task = nullptr;
        if (id)
            m_queue_tasks.wait_and_pop(task);
        else
            m_queue_static_tasks.wait_and_pop(task);
        if (task->getType() == EnumTaskType::LOCAL_TASK || task->getType() == EnumTaskType::STATIC_RESOURCES_TASK) {
            ResponseBase response = task->getHandler()(task->getRequest());
            task->setCode(response.getCode());
            if (task->getRequest()->getMethod() == "HEAD") {
                prepare_head_response(response);
            } else {
                bool client_support_gzip_encoding = check_client_support_gzip_encoding(task);
                bool client_request_range = check_client_request_range(task);
                if (client_request_range) {
                    try {
                        std::vector<std::pair<size_t, size_t>> ranges = parseRangesRequest(
                                task->getRequest()->getHeaderRef("range"), response.getBody().size() - 1);
                        prepare_range_response(response, ranges);
                    } catch (OnyxupException &ex) {
                        prepare_range_not_satisfiable_response(response);
                    }
                } else if (response.isCompress() && client_support_gzip_encoding)
                    prepare_compress_response(response);
                else if (task->getType() == EnumTaskType::STATIC_RESOURCES_TASK && m_compress_static_resources)
                    prepare_compress_response(response);
                else
                    prepare_default_response(response);
            }
            task->setResponseData(response);
        }
        m_queue_performed_tasks.push(task);
    }
}

onyxup::HttpServer::HttpServer(int port, size_t number_threads) : m_number_threads(number_threads) {

#ifdef DEBUG_MODE
    plog::init(plog::debug, &consoleAppender);
#else
    plog::init(plog::info, &consoleAppender);
#endif

    m_path_to_configuration_file = "/var/onyxup/config.json";
    auto settings = parse_configuration_file(m_path_to_configuration_file);

    if (settings.find("server") != settings.end()) {
        json json_server = settings["server"];
        try {
            if (json_server.find("threads") != json_server.end())
                m_number_threads = settings["server"]["threads"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> threads должно быть целым";
        }
        try {
            if (json_server.find("max_connection") != json_server.end())
                m_max_connection = settings["server"]["max_connection"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> max_connection должно быть целым";
        }
        try {
            if (json_server.find("max_input_length_buffer") != json_server.end())
                m_max_input_length_buffer = settings["server"]["max_input_length_buffer"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> max_input_length_buffer должно быть целым";
        }
        try {
            if (json_server.find("max_output_length_buffer") != json_server.end())
                m_max_output_length_buffer = settings["server"]["max_output_length_buffer"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> max_output_length_buffer должно быть целым";
        }
        try {
            if (json_server.find("time_limit_request_seconds") != json_server.end())
                m_time_limit_request_seconds = settings["server"]["time_limit_request_seconds"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> time_limit_request_seconds должно быть целым";
        }
        try {
            if (json_server.find("limit_local_tasks") != json_server.end())
                m_limit_local_tasks = settings["server"]["limit_local_tasks"].get<int>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле server -> limit_local_tasks должно быть целым";
        }
    }
    if (settings.find("static-resources") != settings.end()) {
        json json_static_resources = settings["static-resources"];
        try {
            if (json_static_resources.find("directory") != json_static_resources.end())
                m_path_to_static_resources = settings["static-resources"]["directory"].get<std::string>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле static-resources -> directory должно быть строковым";
        }
        try {
            if (json_static_resources.find("compress") != json_static_resources.end())
                m_compress_static_resources = settings["static-resources"]["compress"].get<bool>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле static-resources -> compress должно быть булевым";
        }
        try {
            if (json_static_resources.find("cache") != json_static_resources.end())
                m_cached_static_resources = settings["static-resources"]["cache"].get<bool>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле static-resources -> cache должно быть булевым";
        }
    }
    if (settings.find("statistics") != settings.end()) {
        try {
            m_statistics_enable = settings["enable"].get<bool>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле statistics -> enable должно быть булевым";
        }
        try {
            m_statistics_url = settings["url"].get<std::string>();
        } catch (json::exception &ex) {
            LOGE << "Ошибка чтения конфигурационного файла. Поле statistics -> url должно быть строковым";
        }
    }


    m_buffers = new PtrBuffer[m_max_connection];
    m_requests = new PtrRequest[m_max_connection];
    statistics_service.reset(new StatisticsService(m_buffers, m_requests, m_max_connection));
    m_alive_sockets.resize(m_max_connection);
    for (size_t i = 0; i < m_max_connection; i++) {
        m_buffers[i] = nullptr;
        m_requests[i] = nullptr;
        m_alive_sockets[i] = std::chrono::steady_clock::now();
    }

    m_map_mime_types = MimeType::generateMimeTypeMap();
    if (m_number_threads < 2)
        m_number_threads = 2;

    m_pool_threads.resize(m_number_threads);

    for (size_t i = 0; i < m_number_threads; i++) {
        std::thread t(&HttpServer::handler_tasks, this, i);
        m_pool_threads[i] = std::move(t);
    }

    struct sockaddr_in server_addr;
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd < 0) {
        LOGE << "Не возможно создать серверный сокет";
        throw OnyxupException("Не возможно создать серверный сокет");
    }
    int enable = 1;
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        close(m_fd);
        LOGE << "Не возможно создать серверный сокет. Ошибка " << errno;
        throw OnyxupException("Не возможно создать серверный сокет");
    }
    if (set_non_blocking_mode_socket(m_fd) == -1) {
        LOGE << "Не возможно создать серверный сокет. Ошибка " << errno;
        throw OnyxupException("Не возможно создать серверный сокет");
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    server_addr.sin_port = htons(port);

    if (bind(m_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        close(m_fd);
        LOGE << "Не возможно выполнить операцию bind. Ошибка " << errno;
        throw OnyxupException("Не возможно создать серверный сокет");
    }

    if (listen(m_fd, SOMAXCONN) < 0) {
        LOGE << "Не возможно выполнить операцию listen. Ошибка " << errno;
        throw OnyxupException("Не возможно создать серверный сокет");
    };

    m_e_poll_fd = epoll_create1(0);
    if (m_e_poll_fd == -1) {
        close(m_fd);
        LOGE << "Не возможно создать epoll. Ошибка " << errno;
        throw OnyxupException("Не возможно создать серверный сокет");
    }

    struct epoll_event event;
    event.data.fd = m_fd;
    event.events = EPOLLIN;

    epoll_ctl(m_e_poll_fd, EPOLL_CTL_ADD, m_fd, &event);
}

void onyxup::HttpServer::run() noexcept {
    sockaddr_in peer_addr;
    int address_length = sizeof(peer_addr);
    struct epoll_event events[m_max_events_epoll];
    struct epoll_event event;

    /*
     * Подключение сбора статистики
     */
    if (m_statistics_enable) {
        this->addRoute("GET", m_statistics_url.c_str(), [](PtrCRequest request) -> ResponseBase {
            return statistics_service->callback(request);
        }, EnumTaskType::LOCAL_TASK);
    }

    for (;;) {
        static size_t counter_check_limit_time_request = 0;
        std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

        if (m_statistics_enable)
            statistics_service->setCurrentNumberTasks(m_queue_tasks.size() + m_queue_static_tasks.size());
        for (size_t i = counter_check_limit_time_request;
             i < m_max_connection && i < counter_check_limit_time_request + 100; i++) {
            if (m_requests[i]) {
                if (std::chrono::duration_cast<std::chrono::seconds>(now - m_alive_sockets[i]).count() >
                    HttpServer::m_time_limit_request_seconds) {
                    if (m_requests[i]->getFullURI().empty())
                        closeAllSocketsAndClearData(m_requests[i]->getFD());
                    else {
                        ResponseBase response = std::move(onyxup::Response408());
                        response.appendHeader("Content-Length", std::to_string(response.getBody().size()));
                        std::string str = response.to_string();
                        writeToOutputBuffer(m_requests[i]->getFD(), str.c_str(), str.length());
                        m_requests[i]->setClosingConnect(true);
                        m_alive_sockets[m_requests[i]->getFD()] = std::chrono::steady_clock::now();
                        LOGI << m_requests[i]->getMethod() << " " << m_requests[i]->getFullURI() << " "
                             << ResponseState::RESPONSE_STATE_METHOD_REQUEST_TIMEOUT_CODE;
                    }
                }
            }
        }
        counter_check_limit_time_request = (counter_check_limit_time_request + 100) % m_max_connection;

        while (!m_queue_performed_tasks.empty()) {
            PtrTask task = nullptr;
            if (m_queue_performed_tasks.try_pop(task)) {
                /*
                 * Проверяем что данный сокет еще жив
                */
                if (task->getTimePoint() == m_alive_sockets[task->getFD()]) {
                    int code = writeToOutputBuffer(task->getFD(), task->getResponseData().c_str(),
                                                   task->getResponseData().size());
                    if (code == ResponseState::RESPONSE_STATE_PAYLOAD_TOO_LARGE_CODE)
                        LOGI << task->getRequest()->getMethod() << " " << task->getRequest()->getFullURI() << " "
                             << ResponseState::RESPONSE_STATE_PAYLOAD_TOO_LARGE_CODE;
                    else LOGI << task->getRequest()->getMethod() << " " << task->getRequest()->getFullURI() << " "
                              << task->getCode();
                }
                delete task;
            }
        }
        int fds = epoll_wait(m_e_poll_fd, events, m_max_events_epoll, 100);
        for (int i = 0; i < fds; i++) {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (events[i].events & EPOLLRDHUP)) {
                closeAllSocketsAndClearData(events[i].data.fd);
                continue;
            }
            if (events[i].data.fd == m_fd) {
                int conn_sock = accept(m_fd, (struct sockaddr *) &peer_addr, (socklen_t *) &address_length);
                if (conn_sock > (int) m_max_connection - 1) {
                    closeSocket(conn_sock);
                    LOGE << "Превышено максимальное количество соединений на сервере";
                    continue;
                }
                if (conn_sock == -1) {
                    closeSocket(conn_sock);
                    LOGE << "Не возможно принять соединение на сервере. Ошибка " << errno;
                    continue;
                }
                if (set_non_blocking_mode_socket(conn_sock) == -1) {
                    closeSocket(conn_sock);
                    LOGE << "Не возможно перевести сокет в неблокирующий режим. Ошибка " << errno;
                    continue;
                }
                event.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
                event.data.fd = conn_sock;
                if (epoll_ctl(m_e_poll_fd, EPOLL_CTL_ADD, conn_sock, &event) == -1) {
                    closeSocket(conn_sock);
                    LOGE << "Не возможно добавить файловый дескриптор в epoll. Ошибка " << errno;
                    continue;
                }

                if (m_statistics_enable)
                    statistics_service->addTotalNumberConnectionsAccepted();

                m_alive_sockets[conn_sock] = std::chrono::steady_clock::now();

                /*
                 * Подготавливаем входной и выходной буфер, выделяем память
                 */
                m_buffers[conn_sock] = buffer::factoryBuffer(m_max_input_length_buffer, m_max_output_length_buffer);
                if (m_buffers[conn_sock] == nullptr) {
                    LOGE << "Ошибка выделение памяти";
                    closeSocket(conn_sock);
                    continue;
                }

                /*
                 * Подготавливаем request
                 */

                m_requests[conn_sock] = request::factoryRequest();
                if (m_requests[conn_sock] == nullptr) {
                    closeSocket(conn_sock);
                    delete m_buffers[conn_sock];
                    m_buffers[conn_sock] = nullptr;
                    continue;
                }
                m_requests[conn_sock]->setFD(conn_sock);
                m_requests[conn_sock]->setMaxOutputLengthBuffer(m_max_output_length_buffer);
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
                    PtrBuffer buffer = m_buffers[events[i].data.fd];
                    if (buffer->getInBufferPosition() + res >= m_max_input_length_buffer) {
                        LOGD << "Превышен размер входного буфера";

                        Response413 response = Response413();
                        response.appendHeader("Content-Length",
                                              std::to_string(response.getBody().size()));
                        std::string str = response.to_string();
                        writeToOutputBuffer(events[i].data.fd, str.c_str(), str.length());
                        m_requests[events[i].data.fd]->setClosingConnect(true);
                        m_requests[events[i].data.fd]->setHeaderAccept(true);
                        continue;
                    }
                    buffer->appendInBuffer(data, res);
                    /*
                     * Парсим http запрос
                     */

                    const char *method, *uri;
                    phr_header headers[100];
                    size_t method_len, uri_len;
                    int version;
                    size_t num_headers = sizeof(headers) / sizeof(headers[0]);
                    int parse_http_result = phr_parse_request(buffer->getInBuffer(),
                                                              buffer->getInBufferPosition() + 1,
                                                              &method,
                                                              &method_len, &uri, &uri_len, &version, headers,
                                                              &num_headers, 0);
                    if (parse_http_result > 0) {
                        PtrRequest request = m_requests[events[i].data.fd];

                        if (!request->isHeaderAccept()) {
                            request->setHeaderAccept(true);
                            request->setFullURI(uri, uri_len);
                            request->setMethod(method, method_len);

                            for (size_t j = 0; j < num_headers; j++) {
                                std::string key(headers[j].name, (int) headers[j].name_len);
                                std::transform(key.begin(), key.end(), key.begin(), tolower);
                                std::string value(headers[j].value, (int) headers[j].value_len);
                                request->appendHeader(key, value);
                            }
                            /*
                             * Получаем параметры строки запроса
                             */
                            parseParamsRequest(request, uri_len);

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
                                if (len && buffer->getInBufferPosition() == (parse_http_result + len)) {
                                    request->setBodyAccept(true);
                                    request->setBody(buffer->getInBuffer() + parse_http_result, len);
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
                            if (epoll_ctl(m_e_poll_fd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1) {
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
                            task->setTimePoint(m_alive_sockets[events[i].data.fd]);
                            /*
                             * В зависимости от типа задачи направляем в соответствующий поток
                             */
                            if (task->getType() == EnumTaskType::LOCAL_TASK) {
                                if (m_queue_tasks.size() > HttpServer::m_limit_local_tasks) {
                                    ResponseBase response = onyxup::Response503();
                                    response.appendHeader("Content-Length",
                                                          std::to_string(response.getBody().size()));
                                    std::string str = response.to_string();
                                    writeToOutputBuffer(events[i].data.fd, str.c_str(), str.size());
                                    LOGI << request->getMethod() << " " << request->getFullURI() << " "
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
                            statistics_service->addTotalNumberClientRequests();
                        } else {
                            ResponseBase response = onyxup::Response404();
                            response.appendHeader("Content-Length", std::to_string(response.getBody().size()));
                            std::string str = response.to_string();
                            writeToOutputBuffer(events[i].data.fd, str.c_str(), str.size());
                            LOGI << request->getMethod() << " " << request->getFullURI() << " "
                                 << ResponseState::RESPONSE_STATE_NOT_FOUND_CODE;
                            statistics_service->addTotalNumberClientRequests();
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
                if (events[i].events & EPOLLOUT && m_buffers[events[i].data.fd]->getBytesToSend() > 0 &&
                    m_requests[events[i].data.fd]->isHeaderAccept()) {
                    if (m_requests[events[i].data.fd]->isBodyExists() &&
                        !m_requests[events[i].data.fd]->isBodyAccept())
                        continue;
                    PtrBuffer buffer = m_buffers[events[i].data.fd];
                    int res = send(events[i].data.fd, buffer->getOutBuffer() + buffer->getOutBufferPosition(),
                                   buffer->getBytesToSend(), 0);
                    if (res == -1) {
                        closeAllSocketsAndClearData(events[i].data.fd);
                        continue;
                    }
                    buffer->setBytesToSend(buffer->getBytesToSend() - res);
                    buffer->setOutBufferPosition(buffer->getOutBufferPosition() + res);
                    if (buffer->getBytesToSend() == 0) {
                        try {
                            if (m_requests[events[i].data.fd]->isClosingConnect()) {
                                closeAllSocketsAndClearData(events[i].data.fd);
                            } else if (m_requests[events[i].data.fd]->getHeader("connection") == "Keep-Alive") {
                                m_buffers[events[i].data.fd]->clear();
                                m_requests[events[i].data.fd]->clear();
                                struct epoll_event event;
                                event.data.fd = events[i].data.fd;
                                event.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP;
                                if (epoll_ctl(m_e_poll_fd, EPOLL_CTL_MOD, events[i].data.fd, &event) == -1) {
                                    closeAllSocketsAndClearData(events[i].data.fd);
                                    LOGE << "Не возможно модифицировать файловый дескриптор в epoll. Ошибка " << errno;
                                    continue;
                                }
                                m_alive_sockets[events[i].data.fd] = std::chrono::steady_clock::now();
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
    for (size_t i = 0; i < m_number_threads; i++)
        m_pool_threads[i].join();

    for (size_t i = 0; i < m_max_connection; i++)
        closeAllSocketsAndClearData(i);
    shutdown(m_fd, SHUT_RD);
    close(m_fd);
    delete[] m_buffers;
    delete[] m_requests;
}

onyxup::ResponseBase onyxup::HttpServer::default_callback_static_resources(onyxup::PtrCRequest request) {
    /*
     * Определяем content type по расширению файла
     */
    char content_type_key[10] = {'\0'};
    char *token_dot = strrchr((char *) request->getFullURI().c_str(), '.');
    if (token_dot == NULL)
        return onyxup::Response404();
    if (strlen(token_dot + 1) > sizeof(content_type_key) - 1) {
        return onyxup::Response404();
    }
    strcat(content_type_key, token_dot + 1);
    std::unordered_map<std::string, std::string>::iterator it = m_map_mime_types.find(
            content_type_key);
    if (it == m_map_mime_types.end()) {
        return onyxup::Response404();
    }

    /*
     * Проверяем кеш иначе получаем ресурс и заносим в кеш
     */
    if (m_cached_static_resources) {
        std::unordered_map<std::string, ResponseBase>::iterator iter = m_map_cached_static_resources.find(
                request->getURIRef());
        if (iter == m_map_cached_static_resources.end()) {
            char path_to_file[120];
            snprintf(path_to_file, sizeof(path_to_file), "%s%s",
                     m_path_to_static_resources.c_str(), request->getURIRef().c_str());

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
                m_map_cached_static_resources[request->getURIRef()] = response;
                return response;
            } else {
                return onyxup::Response404();
            }
        } else {
            /*
             * Берем из кеша
             */
            ResponseBase response = m_map_cached_static_resources[request->getURIRef()];
            return response;
        }
    } else {
        char path_to_file[120];
        snprintf(path_to_file, sizeof(path_to_file), "%s%s",
                 m_path_to_static_resources.c_str(), request->getURIRef().c_str());

        std::ifstream file(path_to_file, std::ios::in | std::ifstream::binary);
        if (file.good()) {
            std::string buf = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();

            ResponseBase response(ResponseState::RESPONSE_STATE_OK_CODE, ResponseState::RESPONSE_STATE_OK_MSG,
                                  it->second.c_str(), buf);
            /*
             * Отправляем данные в кеш
             */
            m_map_cached_static_resources[request->getURIRef()] = response;
            return response;
        } else {
            return onyxup::Response404();
        }
    }

}

std::unordered_map<std::string, std::string> onyxup::HttpServer::urlencoded(const std::string &src) {
    std::unordered_map<std::string, std::string> params;
    std::istringstream iss(src);
    std::string token;
    while (std::getline(iss, token, '&')) {
        size_t pos = token.find("=");
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1, token.size());
            if (!key.empty() && !value.empty())
                params[key] = value;
        }
    }
    return params;
}

std::unordered_map<std::string, onyxup::MultipartFormDataObject>
onyxup::HttpServer::multipartFormData(PtrCRequest request) {
    std::unordered_map<std::string, MultipartFormDataObject> form_fields;
    std::string boundary;
    try {
        std::string content_type = request->getHeaderRef("content-type");
        size_t pos = content_type.find("boundary=");
        if (content_type.find("multipart/form-data") == std::string::npos || pos == std::string::npos)
            return form_fields;
        boundary = content_type.substr(pos + strlen("boundary="));
        if (boundary.empty())
            return form_fields;
    } catch (std::out_of_range) {
        return form_fields;
    }
    size_t pos = request->getBodyRef().find(boundary, 0);
    while (pos != std::string::npos && pos < request->getBodyRef().size()) {
        size_t position_content_disposition = request->getBodyRef().find("Content-Disposition: form-data",
                                                                         pos + boundary.length());
        if (position_content_disposition == std::string::npos)
            break;
        std::vector<char> characters;
        std::string name;
        size_t position_name = request->getBodyRef().find("name=", pos + boundary.length());
        if (position_name != std::string::npos) {
            position_name += strlen("name=");
            while (position_name < request->getBodyRef().length() &&
                   !isspace(request->getBodyRef()[position_name]) &&
                   request->getBodyRef()[position_name] != ';') {
                characters.push_back(request->getBodyRef()[position_name]);
                position_name++;
            }
            if (!characters.empty())
                name = trim_characters(characters, {'"', ' '});
        }
        characters.clear();
        std::string file_name;
        size_t position_filename = request->getBodyRef().find("filename=", pos + boundary.length());
        if (position_filename != std::string::npos) {
            position_filename += strlen("filename=");
            while (position_filename < request->getBodyRef().length() &&
                   !isspace(request->getBodyRef()[position_filename]) &&
                   request->getBodyRef()[position_filename] != ';') {
                characters.push_back(request->getBodyRef()[position_filename]);
                position_filename++;
            }
            if (!characters.empty())
                file_name = trim_characters(characters, {'"', ' '});
        }
        characters.clear();
        std::string content_type;
        size_t position_content_type = request->getBodyRef().find("Content-Type:", pos + boundary.length());
        if (position_content_type != std::string::npos) {
            position_content_type += strlen("Content-Type:");
            while (position_content_type < request->getBodyRef().length() &&
                   request->getBodyRef()[position_content_type] != '\n' &&
                   request->getBodyRef()[position_content_type] != '\r') {
                characters.push_back(request->getBodyRef()[position_content_type]);
                position_content_type++;
            }
            if (!characters.empty())
                content_type = trim_characters(characters, {'"', ' '});
        }
        size_t position_data = request->getBodyRef().find("\r\n\r\n", pos + boundary.length());
        std::vector<char> data;
        pos = request->getBodyRef().find(boundary, pos + boundary.length());
        if (position_data != std::string::npos && pos != std::string::npos) {
            std::copy(request->getBodyRef().begin() + position_data + strlen("\r\n\r\n"),
                      request->getBodyRef().begin() + pos - strlen("\r\n"), std::back_inserter(data));
        }
        if (!name.empty())
            form_fields.emplace(std::piecewise_construct, std::make_tuple(name),
                                std::make_tuple(name, file_name, content_type, data));
    }
    return form_fields;
}

int onyxup::HttpServer::getTimeLimitRequestSeconds() {
    return m_time_limit_request_seconds;
}

void onyxup::HttpServer::setTimeLimitRequestSeconds(int time_limit_request_seconds) {
    m_time_limit_request_seconds = time_limit_request_seconds;
}

int onyxup::HttpServer::getLimitLocalTasks() {
    return m_limit_local_tasks;
}

void onyxup::HttpServer::setLimitLocalTasks(int limit_local_tasks) {
    m_limit_local_tasks = limit_local_tasks;
}

void onyxup::HttpServer::setCachedStaticResources(bool cached_static_resources) {
    m_cached_static_resources = cached_static_resources;
}

void onyxup::HttpServer::setPathToConfigurationFile(const std::string &path_to_configuration_file) {
    m_path_to_configuration_file = path_to_configuration_file;
}

void onyxup::HttpServer::setStatisticsEnable(bool enable) {
    m_statistics_enable = enable;
}

void onyxup::HttpServer::setStatisticsUrl(const std::string &url) {
    m_statistics_url = url;
}

void onyxup::HttpServer::closeAllSocketsAndClearData(int fd) {
    shutdown(fd, SHUT_RDWR);
    close(fd);
    delete m_buffers[fd];
    delete m_requests[fd];
    m_alive_sockets[fd] = std::chrono::steady_clock::now();
    m_buffers[fd] = nullptr;
    m_requests[fd] = nullptr;
    statistics_service->addTotalNumberConnectionsProcessed();
}