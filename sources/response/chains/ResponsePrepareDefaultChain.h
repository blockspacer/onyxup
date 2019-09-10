#pragma once

#include "IResponsePrepareChain.h"

static void prepareDefaultResponse(onyxup::ResponseBase &response) {
    response.addHeader("Content-Length", std::to_string(response.getBody().size()));
}

namespace onyxup {
    class ResponsePrepareDefaultChain : public IResponsePrepareChain {
    public:
        
       
        virtual void execute(PtrTask task, onyxup::ResponseBase &response) override {
            prepareDefaultResponse(response);
            if (nextChain != nullptr)
                nextChain->execute(task, response);
        }
    };
}
