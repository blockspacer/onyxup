#pragma once

#include "../request/request.h"
#include "response-base.h"
#include "../mime/types.h"
#include "response-states.h"

namespace onyxup {

    class Response413 : public onyxup::ResponseBase {
    public:
        Response413() : ResponseBase(ResponseState::RESPONSE_STATE_PAYLOAD_TOO_LARGE_CODE, ResponseState::RESPONSE_STATE_METHOD_PAYLOAD_TOO_LARGE_MSG, MimeType::MIME_TYPE_TEXT_HTML, 
                "<html><head><title>413</title></head><body bgcolor=\"white\"><center><h1>Payload Too Large 413</h1></center><center>onyxup</center></body></html>",
                                     false
                ){
            
        }
    };
}