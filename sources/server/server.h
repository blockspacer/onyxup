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
#include <algorithm>
#include <mutex>
#include <thread>
#include <iterator>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <queue>

#include "../buffer/buffer.h"
#include "../request/request.h"
#include "../exception/exception.h"
#include "../route/route.h"
#include "../task/task.h"
#include "../mime/types.h"
#include "../httpparser/picohttpparser.h"
#include "../response/response-404.h"
#include "../response/response-413.h"
#include "../response/response-408.h"
#include "../response/response-503.h"
#include "../response/response-base.h"
#include "../response/response-states.h"
#include "../plog/Log.h"
#include "../plog/Appenders/ColorConsoleAppender.h"
#include "../queue/thread-safe-queue.h"
#include "../multipart/MultipartFormDataObject.h"
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
        int m_fd;
        int m_e_poll_fd;
        size_t m_number_threads;
        std::vector<std::thread> m_pool_threads;

        size_t m_max_connection = 10000;
        size_t m_max_events_epoll = 100;
        size_t m_max_input_length_buffer = 1024 * 1024 * 2;
        size_t m_max_output_length_buffer = 1024 * 1024 * 2;
        PtrBuffer * m_buffers;
        PtrRequest * m_requests;
        std::vector<std::chrono::time_point<std::chrono::steady_clock>> m_alive_sockets;

        std::vector<Route> m_routes;

        ThreadSafeQueue<PtrTask> m_queue_tasks;
        ThreadSafeQueue<PtrTask> m_queue_static_tasks;
        ThreadSafeQueue<PtrTask> m_queue_performed_tasks;

        static bool m_statistics_enable;
        static std::string m_statistics_url;
        static int m_time_limit_request_seconds;
        static int m_limit_local_tasks;
        static std::string m_path_to_static_resources;
        static bool m_compress_static_resources;
        static bool m_cached_static_resources;
        static std::string m_path_to_configuration_file;
        static std::unordered_map<std::string, std::string> m_map_mime_types;
        static std::unordered_map<std::string, ResponseBase> m_map_cached_static_resources;

        void closeAllSocketsAndClearData(int fd);

        inline void closeSocket(int fd) {
            shutdown(fd, SHUT_RDWR);
            close(fd);
        }

        inline void addTask(PtrTask task) {
            if(task->getType() == EnumTaskType::LOCAL_TASK)
                m_queue_tasks.push(task);
            else if(task->getType() == EnumTaskType::STATIC_RESOURCES_TASK)
                m_queue_static_tasks.push(task);
        }

        void handler_tasks(int id);
        int writeToOutputBuffer(int fd, const char * data, size_t len) noexcept ;
        PtrTask dispatcher(PtrRequest request) noexcept;

    protected:
        static void parseParamsRequest(onyxup::PtrRequest request, size_t uri_len) noexcept;
        static std::vector<std::pair<size_t , size_t>> parseRangesRequest(const std::string & src, size_t length);

    public:
        HttpServer(int port, size_t n);

        HttpServer(const HttpServer &) = delete;
        ~HttpServer();

        void run() noexcept ;
        void addRoute(const std::string & method, const char * regex, std::function<ResponseBase(PtrCRequest request) > handler, EnumTaskType type) noexcept ;

        static std::unordered_map<std::string, std::string> urlencoded(const std::string & src);
        static std::unordered_map<std::string, MultipartFormDataObject> multipartFormData(PtrCRequest request);

        static void setPathToStaticResources(const std::string & path) {
            m_path_to_static_resources = path;
        }

        static void setCompressStaticResources(bool compress){
            m_compress_static_resources = compress;
        }

        static ResponseBase default_callback_static_resources(PtrCRequest request);

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

        void setMaxInputLengthBuffer(size_t length);

        void setMaxOutputLengthBuffer(size_t length);

    };

}