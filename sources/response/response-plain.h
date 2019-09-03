#pragma once

#include "../request/request.h"
#include "response-base.h"
#include "../mime/types.h"
#include "response-states.h"

namespace onyxup {

    class ResponsePlain : public onyxup::ResponseBase {
    public:
        ResponsePlain(const std::string & body, bool compress = false) : ResponseBase(ResponseState::RESPONSE_STATE_OK_CODE, ResponseState::RESPONSE_STATE_OK_MSG, MimeType::MIME_TYPE_TEXT_PLAIN, body, compress){
            
        }
    };
}