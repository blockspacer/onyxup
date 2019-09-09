#pragma once

#include "../../task/task.h"

namespace onyxup {
    class IResponsePrepareChain {
    protected:
        std::shared_ptr<IResponsePrepareChain> nextChain;
    public:
        IResponsePrepareChain(){
            nextChain = nullptr;
        }    
    
        void setNextHandler(std::shared_ptr<IResponsePrepareChain> handler) {
            nextChain = handler;
        }
    
        virtual void execute(PtrTask task, onyxup::ResponseBase &response) = 0;
        
        virtual ~IResponsePrepareChain(){
            
        }
    };
}
