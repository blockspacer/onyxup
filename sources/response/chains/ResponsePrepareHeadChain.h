#pragma once

#include "IResponsePrepareChain.h"

static void prepareHeadResponse(onyxup::ResponseBase &response) {
    response.addHeader("Content-Length", std::to_string(response.getBody().size()));
    response.setBody("");
}

namespace onyxup {
    class ResponsePrepareHeadChain : public IResponsePrepareChain {
    public:
        virtual void execute(PtrTask task, onyxup::ResponseBase &response) override {
            if(task->getRequest()->getMethod() == "HEAD"){
                prepareHeadResponse(response);
            }else {
                 if (nextChain != nullptr)
                     nextChain->execute(task, response);
            }
        }
    };
}
