#pragma once

#include "../request/request.h"
#include "response-base.h"
#include "../mime/types.h"
#include "response-states.h"

namespace onyxup {

    class Response503 : public onyxup::ResponseBase {
    public:
        Response503() : ResponseBase(ResponseState::RESPONSE_STATE_SERVICE_UNAVAILABLE_CODE, ResponseState::RESPONSE_STATE_SERVICE_UNAVAILABLE_MSG, MimeType::MIME_TYPE_TEXT_HTML,
                "<html><head><title>413</title></head><body bgcolor=\"white\"><center><h1>Service Temporarily Unavailable 503</h1></center><center>onyxup</center></body></html>",
                                     false
                ){
            
        }
    };
}