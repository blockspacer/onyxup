#pragma once

#include "../../task/task.h"

namespace onyxup {
    class IPrepareResponseChain {
    protected:
        std::shared_ptr<IPrepareResponseChain> m_next_prepare_response_chain;
    public:
        IPrepareResponseChain(){
            m_next_prepare_response_chain = nullptr;
        }    
    
        void setNextHandler(std::shared_ptr<IPrepareResponseChain> handler) {
            m_next_prepare_response_chain = handler;
        }
    
        virtual void execute(PtrTask task, onyxup::ResponseBase &response) = 0;
        
        virtual ~IPrepareResponseChain(){
            
        }
    };
}
