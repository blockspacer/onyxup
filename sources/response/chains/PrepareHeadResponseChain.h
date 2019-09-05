#pragma once

#include "IPrepareResponseChain.h"

static void prepare_head_response(onyxup::ResponseBase &response) {
    response.appendHeader("Content-Length", std::to_string(response.getBody().size()));
    response.setBody("");
}

namespace onyxup {
    class PrepareHeadResponseChain : public IPrepareResponseChain {
    public:
        virtual void execute(PtrTask task, onyxup::ResponseBase &response) override {
            if(task->getRequest()->getMethod() == "HEAD"){
                prepare_head_response(response);
            }else {
                 if (m_next_prepare_response_chain != nullptr)
                     m_next_prepare_response_chain->execute(task, response);
            }
        }
    };
}
