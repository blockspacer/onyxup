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
        std::string m_method;
        regex_t m_pregex;
        std::function<ResponseBase(PtrCRequest request) > m_handler;
        EnumTaskType m_type;
    public:

        Route(const std::string & method, const char * regex, std::function<ResponseBase(PtrCRequest) > & handler, EnumTaskType type) : m_method(method), m_handler(handler){
            m_type = type;
            int err;
            err = regcomp(&m_pregex, regex, REG_EXTENDED);
            if (err != 0)
                throw OnyxupException("Ошибка создания Route");
        }

        inline std::string getMethod() const {
            return m_method;
        }

        inline regex_t getPregex() const {
            return m_pregex;
        }

        inline EnumTaskType getTaskType() const {
            return m_type;
        }
        
        std::function<ResponseBase(PtrCRequest) > getHandler() const {
            return m_handler;
        }



    };
}