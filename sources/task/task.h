#pragma once

#include <functional>

#include "../request/request.h"
#include "../response/response-base.h"

namespace onyxup {

    class Task;

    using PtrTask = Task*;

    enum class EnumTaskType {
        LOCAL_TASK = 1,
        STATIC_RESOURCES_TASK,
        JSON_RPC_TASK
    };

    PtrTask factoryTask();

    class Task {
    private:
        int m_fd;
        onyxup::PtrRequest m_request;
        std::function<ResponseBase(PtrCRequest request)> m_handler;
        EnumTaskType m_type;
        std::string m_response_data;
        std::chrono::time_point<std::chrono::steady_clock> m_time_point;
        int m_code;
    public:

        Task() = default;
        
        ~Task(){
            delete m_request;
        }

        inline void setTimePoint(std::chrono::time_point<std::chrono::steady_clock> & time_point){
            m_time_point = time_point;
        }

        inline void setFD(int fd) {
            m_fd = fd;
        }
        
        inline EnumTaskType getType() const {
            return m_type;
        }

        inline void setType(EnumTaskType type) {
            m_type = type;
        }
        
        inline int getFD() const {
            return m_fd;
        }

        inline std::string getResponseData() const {
            return m_response_data;
        }
        
        inline std::chrono::time_point<std::chrono::steady_clock> getTimePoint() const {
            return m_time_point;
        }
        
        void setHandler(const std::function<ResponseBase(PtrCRequest) > & handler) {
            m_handler = handler;
        }

        std::function<ResponseBase(PtrCRequest) > getHandler() {
            return m_handler;
        }
        
        inline onyxup::PtrRequest getRequest() const {
            return m_request;
        }
        
        inline void setResponseData(std::string && response_data) {
            m_response_data = response_data;
        }

        inline void setRequest(onyxup::PtrRequest request) {
            m_request = request;
        }

        void setCode(int code) {
            m_code = code;
        }

        int getCode() const {
            return m_code;
        }

    };

}
