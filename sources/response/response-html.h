#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "../request/request.h"
#include "response-base.h"
#include "../mime/types.h"
#include "response-states.h"

namespace onyxup {

    class ResponseHtml: public ResponseBase {
    private:

        std::string file_reader(const std::string & name) {
            std::stringstream buffer;
            std::string line;
            std::ifstream file(name.c_str());
            if (file.is_open()) {
                while (file.good()) {
                    getline(file, line);
                    buffer << line;
                }
                file.close();
            }
            return buffer.str();
        }
    public:

        ResponseHtml(const std::string & html_file, bool compress = false) : ResponseBase(ResponseState::RESPONSE_STATE_OK_CODE, ResponseState::RESPONSE_STATE_OK_MSG, MimeType::MIME_TYPE_TEXT_HTML, compress) {
            setBody(file_reader(html_file));
        }
    };
}