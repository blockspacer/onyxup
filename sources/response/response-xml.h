#pragma once

#include "../request/request.h"
#include "response-base.h"
#include "../mime/types.h"
#include "response-states.h"

namespace onyxup {

    class ResponseXML : public onyxup::ResponseBase {
    public:
        ResponseXML(const std::string & body, bool compress = false) : ResponseBase(ResponseState::RESPONSE_STATE_OK_CODE, ResponseState::RESPONSE_STATE_OK_MSG, MimeType::MIME_TYPE_APPLICATION_XML, body, compress){
            
        }
    };
}