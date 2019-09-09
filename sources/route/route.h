#pragma once

#include <regex.h>
#include <functional>

#include "../request/request.h"
#include "../response/response-base.h"
#include "../task/task.h"
#include "../exception/exception.h"

namespace onyxup {

    class Route {
    private:
        std::string method;
        regex_t pregex;
        std::function<ResponseBase(PtrCRequest request) > handler;
        EnumTaskType type;
    public:

        Route(const std::string & method, const char * regex, std::function<ResponseBase(PtrCRequest) > & handler, EnumTaskType type) : method(method), handler(handler){
            this->type = type;
            int err;
            err = regcomp(&pregex, regex, REG_EXTENDED);
            if (err != 0)
                throw OnyxupException("Ошибка создания Route");
        }

        inline std::string getMethod() const {
            return method;
        }

        inline regex_t getPregex() const {
            return pregex;
        }

        inline EnumTaskType getTaskType() const {
            return type;
        }
        
        std::function<ResponseBase(PtrCRequest) > getHandler() const {
            return handler;
        }



    };
}