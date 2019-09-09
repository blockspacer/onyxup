#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "../request/request.h"
#include "response-base.h"
#include "../mime/types.h"
#include "response-states.h"
#include "response-404.h"

namespace onyxup {

    class ResponseFile : public ResponseBase {
    private:

        std::string file_reader(const std::string & name) {
            std::string buffer;
            std::ifstream file(name, std::ios::in | std::ifstream::binary);
            if (file.good()) {
                buffer = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                file.close();
            }
            return buffer;
        }
    public:

        ResponseFile(const std::string & file, bool compress = false) : ResponseBase(ResponseState::RESPONSE_STATE_OK_CODE, ResponseState::RESPONSE_STATE_OK_MSG, MimeType::MIME_TYPE_APPLICATION_OCTET_STREAM, compress) {
            setBody(file_reader(file));
            addHeader("Content-Description", "File Transfer");
            addHeader("Content-Transfer-Encoding", "binary");
        }

    };

}