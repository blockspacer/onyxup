#pragma once

#include "IResponsePrepareChain.h"
#include "../../gzip/compress.hpp"

static bool checkSupportGzipEncoding(onyxup::PtrTask task) {
    try {
        if (task->getRequest()->getHeaderRef("accept-encoding").find("gzip") != std::string::npos)
            return true;
    } catch (std::out_of_range &ex) {
        return false;
    }
}

static void prepareCompressResponse(onyxup::ResponseBase &response) {
    response.addHeader("Content-Encoding", "gzip");
    std::string compressed_body = gzip::compress(response.getBody().c_str(), response.getBody().size());
    response.setBody(compressed_body);
    response.addHeader("Content-Length", std::to_string(response.getBody().size()));
}

namespace onyxup {
    class ResponsePrepareCompressChain : public IResponsePrepareChain {
    private:
        bool compressStaticResources;
    public:
        
        ResponsePrepareCompressChain(bool flag = false) : IResponsePrepareChain(), compressStaticResources(flag){
        }
        
        virtual void execute(PtrTask task, onyxup::ResponseBase &response) override {
            if(checkSupportGzipEncoding(task) && response.isCompress())
                prepareCompressResponse(response);
            else if(task->getType() == EnumTaskType::STATIC_RESOURCES_TASK && compressStaticResources)
                prepareCompressResponse(response);
            else {
                if (nextChain != nullptr)
                    nextChain->execute(task, response);
            }
        }
    };
}
