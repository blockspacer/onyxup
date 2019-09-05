#pragma once

#include "IPrepareResponseChain.h"

static void prepare_default_response(onyxup::ResponseBase &response) {
    response.appendHeader("Content-Length", std::to_string(response.getBody().size()));
}

namespace onyxup {
    class PrepareDefaultResponseChain : public IPrepareResponseChain {
    public:
        
       
        virtual void execute(PtrTask task, onyxup::ResponseBase &response) override {
            prepare_default_response(response);
            if (m_next_prepare_response_chain != nullptr)
                m_next_prepare_response_chain->execute(task, response);
        }
    };
}
