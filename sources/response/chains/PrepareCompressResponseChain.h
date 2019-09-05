#pragma once

#include "IPrepareResponseChain.h"
#include "../../gzip/compress.hpp"

static bool check_client_support_gzip_encoding(onyxup::PtrTask task) {
    try {
        if (task->getRequest()->getHeaderRef("accept-encoding").find("gzip") != std::string::npos)
            return true;
    } catch (std::out_of_range &ex) {
        return false;
    }
}

static void prepare_compress_response(onyxup::ResponseBase &response) {
    response.appendHeader("Content-Encoding", "gzip");
    std::string compressed_body = gzip::compress(response.getBody().c_str(), response.getBody().size());
    response.setBody(compressed_body);
    response.appendHeader("Content-Length", std::to_string(response.getBody().size()));
}

namespace onyxup {
    class PrepareCompressResponseChain : public IPrepareResponseChain {
    private:
        bool m_compress_static_resources;
    public:
        
        PrepareCompressResponseChain(bool flag = false) : IPrepareResponseChain(), m_compress_static_resources(flag){
        }
        
        virtual void execute(PtrTask task, onyxup::ResponseBase &response) override {
            if(check_client_support_gzip_encoding(task) && response.isCompress())
                prepare_compress_response(response);
            else if(task->getType() == EnumTaskType::STATIC_RESOURCES_TASK && m_compress_static_resources)
                prepare_compress_response(response);
            else {
                if (m_next_prepare_response_chain != nullptr)
                    m_next_prepare_response_chain->execute(task, response);
            }
        }
    };
}
