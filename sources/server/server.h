#pragma once

#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <list>
#include <functional>
#include <mutex>
#include <thread>
#include <iterator>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <queue>

#include "utils.h"
#include "../buffer/buffer.h"
#include "../request/request.h"
#include "../exception/exception.h"
#include "../route/route.h"
#include "../task/task.h"
#include "../mime/types.h"
#include "../httpparser/picohttpparser.h"
#include "../response/chains/ResponsePrepareHeadChain.h"
#include "../response/chains/ResponsePrepareRangeChain.h"
#include "../response/chains/ResponsePrepareCompressChain.h"
#include "../response/chains/ResponsePrepareDefaultChain.h"
#include "../response/response-404.h"
#include "../response/response-413.h"
#include "../response/response-408.h"
#include "../response/response-503.h"
#include "../response/response-base.h"
#include "../response/response-states.h"
#include "../plog/Log.h"
#include "../plog/Appenders/ColorConsoleAppender.h"
#include "../queue/thread-safe-queue.h"
#include "../json/json.hpp"
#include "../services/statistics/StatisticsService.h"

namespace onyxup {
    
    template <bool useUtcTime = false>
    class Formatter{
    public:
        static plog::util::nstring header()
        {
            return plog::util::nstring();
        }

        static plog::util::nstring format(const plog::Record& record)
        {

            tm t;
            (useUtcTime ? plog::util::gmtime_s : plog::util::localtime_s)(&t, &record.getTime().time);

            plog::util::nostringstream ss;
            ss << t.tm_year + 1900 << "-" << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mon + 1 << PLOG_NSTR("-") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_mday << PLOG_NSTR(" ");
            ss << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_hour << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_min << PLOG_NSTR(":") << std::setfill(PLOG_NSTR('0')) << std::setw(2) << t.tm_sec << PLOG_NSTR(".") << std::setfill(PLOG_NSTR('0')) << std::setw(3) << record.getTime().millitm << PLOG_NSTR(" ");
            ss << std::setfill(PLOG_NSTR(' ')) << std::setw(5) << std::left << severityToString(record.getSeverity()) << PLOG_NSTR(" ");
            ss << PLOG_NSTR("[onyxup] ");
            ss << record.getMessage() << PLOG_NSTR("\n");

            return ss.str();
        }
    };

    class HttpServer {
    private:
        int fd;
        int epollFd;
        size_t numberThreads;
        std::vector<std::thread> threadsPool;

        size_t maxConnection = 10000;
        size_t maxEventsEpoll = 100;
        size_t maxInputBufferLength = 1024 * 1024 * 2;
        size_t maxOutputBufferLength = 1024 * 1024 * 75;
        PtrBuffer * buffers;
        PtrRequest * requests;
        std::vector<std::chrono::time_point<std::chrono::steady_clock>> aliveSockets;

        std::vector<Route> routes;

        ThreadSafeQueue<PtrTask> tasksQueue;
        ThreadSafeQueue<PtrTask> staticTasksQueue;
        ThreadSafeQueue<PtrTask> performedTasksQueue;

        static bool isStatisticsEnable;
        static std::string statisticsUrl;
        static int timeLimitRequestSeconds;
        static int limitLocalTasks;
        static std::string pathToStaticResources;
        static bool isCompressStaticResources;
        static bool isCachedStaticResources;
        static std::string pathToConfigurationFile;
        static std::unordered_map<std::string, std::string> mimeTypesMap;
        static std::unordered_map<std::string, ResponseBase> cachedStaticResourcesMap;

        void closeAllSocketsAndClearData(int fd);

        inline void closeSocket(int fd) {
            shutdown(fd, SHUT_RDWR);
            close(fd);
        }

        inline void addTask(PtrTask task) {
            if(task->getType() == EnumTaskType::LOCAL_TASK)
                tasksQueue.push(task);
            else if(task->getType() == EnumTaskType::STATIC_RESOURCES_TASK)
                staticTasksQueue.push(task);
        }

        void tasksHandler(int id);
        int writeToOutputBuffer(int fd, const char * data, size_t len) noexcept ;
        PtrTask dispatcher(PtrRequest request) noexcept;

    public:
        
        HttpServer(int port, size_t n);

        HttpServer(const HttpServer &) = delete;
        ~HttpServer();

        void run() noexcept ;
        void addRoute(const std::string & method, const char * regex, std::function<ResponseBase(PtrCRequest request) > handler, EnumTaskType type) noexcept ;

        static void setPathToStaticResources(const std::string & path) {
            pathToStaticResources = path;
        }

        static void setCompressStaticResources(bool compress){
            isCompressStaticResources = compress;
        }

        static ResponseBase defaultStaticResourcesCallback(PtrCRequest request);

        static const char * getVersion() {
            return VERSION_APPLICATION;
        }

        static int getTimeLimitRequestSeconds();

        static void setTimeLimitRequestSeconds(int limit);

        static int getLimitLocalTasks();

        static void setLimitLocalTasks(int limit);

        static void setCachedStaticResources(bool flag);

        static void setPathToConfigurationFile(const std::string &file);

        static void setStatisticsEnable(bool enable);

        static void setStatisticsUrl(const std::string & url);

        void setMaxInputBufferLength(size_t len);

        void setMaxOutputBufferLength(size_t len);

    };

}
