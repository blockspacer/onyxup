#pragma once

#include "IPrepareResponseChain.h"
#include "../../response/response-states.h"
#include "../../exception/exception.h"
#include "../../mime/types.h"

static bool check_client_request_range(onyxup::PtrTask task) {
    try {
        if (task->getRequest()->getHeaderRef("range").find("bytes") != std::string::npos)
            return true;
    } catch (std::out_of_range &ex) {
        return false;
    }
}

void static prepare_range_not_satisfiable_response(onyxup::ResponseBase &response) {
    std::ostringstream os;
    os << "*/" << response.getBody().size();
    response.setCode(onyxup::ResponseState::RESPONSE_STATE_RANGE_NOT_SATISFIABLE_CODE);
    response.setCodeMsg(onyxup::ResponseState::RESPONSE_STATE_RANGE_NOT_SATISFIABLE_MSG);
    response.appendHeader("Content-Range", os.str());
    response.setBody("");
}

static void prepare_range_response(onyxup::ResponseBase &response, std::vector<std::pair<size_t, size_t>> &ranges) {
    if (ranges.size() == 1) {
        std::string body;
        std::copy(response.getBody().begin() + ranges[0].first,
                  response.getBody().begin() + ranges[0].second + 1,
                  std::back_insert_iterator<std::string>(body));
        std::ostringstream os;
        os << "bytes " << ranges[0].first << "-" << ranges[0].second << "/"
           << response.getBody().size();
        response.setCode(onyxup::ResponseState::RESPONSE_STATE_PARTIAL_CONTENT_CODE);
        response.setCodeMsg(onyxup::ResponseState::RESPONSE_STATE_PARTIAL_CONTENT_MSG);
        response.appendHeader("Content-Range", os.str());
        response.appendHeader("Content-Length", std::to_string(body.size()));
        response.setBody(body);
    } else {
        size_t length_body = response.getBody().size();
        const char *content_type_body = response.getMime();
        std::ostringstream os;
        response.setCode(onyxup::ResponseState::RESPONSE_STATE_PARTIAL_CONTENT_CODE);
        response.setCodeMsg(onyxup::ResponseState::RESPONSE_STATE_PARTIAL_CONTENT_MSG);
        response.setMime(onyxup::MimeType::MIME_TYPE_MULTIPART_BYTES_RANGES);
        for (size_t i = 0; i < ranges.size(); i++) {
            auto range = ranges[i];
            std::string body;
            std::copy(response.getBody().begin() + range.first,
                      response.getBody().begin() + range.second + 1,
                      std::back_insert_iterator<std::string>(body));
            os << "--3d6b6a416f9b5\r\n" << "Content-Type: " << content_type_body << "\r\n"
               << "Content-Range: bytes " << range.first << "-" << range.second << "/"
               << length_body << "\r\n\r\n" << body;
            if (i < ranges.size() - 1)
                os << "\r\n";
        }
        response.setBody(os.str());
        response.appendHeader("Content-Length", std::to_string(response.getBody().size()));
    }
}

namespace onyxup {
    class PrepareRangeResponseChain : public IPrepareResponseChain {
    public:
        virtual void execute(PtrTask task, onyxup::ResponseBase &response) override {
            if(check_client_request_range(task)){
                try {
                        std::vector<std::pair<size_t, size_t>> ranges = utils::parseRangesRequest(task->getRequest()->getHeaderRef("range"), response.getBody().size() - 1);
                        prepare_range_response(response, ranges);
                    } catch (OnyxupException &ex) {
                        prepare_range_not_satisfiable_response(response);
                    }
            } else {
                 if (m_next_prepare_response_chain != nullptr)
                     m_next_prepare_response_chain->execute(task, response);
            }
        }
    };
}

