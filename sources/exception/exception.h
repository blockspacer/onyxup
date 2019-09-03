#pragma once

#include <exception>
#include <string>

namespace onyxup {

    class OnyxupException : public std::exception {
    private:
        std::string msg_;
    public:

        explicit OnyxupException(const std::string & msg_) : exception(), msg_(msg_) {
        }

        virtual const char* what() const noexcept override {
            return msg_.c_str();
        }

    };
}
