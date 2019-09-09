#pragma once

#include <exception>
#include <string>

namespace onyxup {

    class OnyxupException : public std::exception {
    private:
        std::string msg;
    public:

        explicit OnyxupException(const std::string & msg) : exception(), msg(msg) {
        }

        virtual const char* what() const noexcept override {
            return msg.c_str();
        }

    };
}
