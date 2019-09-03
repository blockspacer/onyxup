#pragma once

#include "../request/request.h"
#include "response-base.h"
#include "../mime/types.h"
#include "response-states.h"

namespace onyxup {

    class Response408 : public onyxup::ResponseBase {
    public:
        Response408() : ResponseBase(ResponseState::RESPONSE_STATE_METHOD_REQUEST_TIMEOUT_CODE, ResponseState::RESPONSE_STATE_METHOD_REQUEST_TIMEOUT_MSG, MimeType::MIME_TYPE_TEXT_HTML,
                "<html><head><title>413</title></head><body bgcolor=\"white\"><center><h1>Request Timeout</h1></center><center>onyxup</center></body></html>",
                                     false
                ){
            
        }
    };
}