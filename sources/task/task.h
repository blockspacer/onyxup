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

    PtrTask taskFactory();

    class Task {
    private:
        int fd;
        onyxup::PtrRequest request;
        std::function<ResponseBase(PtrCRequest request)> handler;
        EnumTaskType type;
        std::string responseData;
        std::chrono::time_point<std::chrono::steady_clock> timePoint;
        int code;
    public:

        Task() = default;
        
        ~Task(){
            delete request;
        }

        inline void setTimePoint(std::chrono::time_point<std::chrono::steady_clock> & timePoint){
            this->timePoint = timePoint;
        }

        inline void setFD(int fd) {
            this->fd = fd;
        }
        
        inline EnumTaskType getType() const {
            return type;
        }

        inline void setType(EnumTaskType type) {
            this->type = type;
        }
        
        inline int getFD() const {
            return fd;
        }

        inline std::string getResponseData() const {
            return responseData;
        }
        
        inline std::chrono::time_point<std::chrono::steady_clock> getTimePoint() const {
            return timePoint;
        }
        
        void setHandler(const std::function<ResponseBase(PtrCRequest) > & handler) {
            this->handler = handler;
        }

        std::function<ResponseBase(PtrCRequest) > getHandler() {
            return handler;
        }
        
        inline onyxup::PtrRequest getRequest() const {
            return request;
        }
        
        inline void setResponseData(std::string && data) {
            responseData = data;
        }

        inline void setRequest(onyxup::PtrRequest request) {
            this->request = request;
        }

        void setCode(int code) {
            this->code = code;
        }

        int getCode() const {
            return code;
        }

    };

}
