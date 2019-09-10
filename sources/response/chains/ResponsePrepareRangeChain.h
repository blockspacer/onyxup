#pragma once

#include "IResponsePrepareChain.h"
#include "../../response/response-states.h"
#include "../../exception/exception.h"
#include "../../mime/types.h"

static bool checkRequestRange(onyxup::PtrTask task) {
    try {
        if (task->getRequest()->getHeaderRef("range").find("bytes") != std::string::npos)
            return true;
    } catch (std::out_of_range &ex) {
        return false;
    }
}

void static prepareRangeNotSatisfiableResponse(onyxup::ResponseBase &response) {
    std::ostringstream os;
    os << "*/" << response.getBody().size();
    response.setCode(onyxup::ResponseState::RESPONSE_STATE_RANGE_NOT_SATISFIABLE_CODE);
    response.setCodeMsg(onyxup::ResponseState::RESPONSE_STATE_RANGE_NOT_SATISFIABLE_MSG);
    response.addHeader("Content-Range", os.str());
    response.setBody("");
}

static void prepareRangeResponse(onyxup::ResponseBase &response, std::vector<std::pair<size_t, size_t>> &ranges) {
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
        response.addHeader("Content-Range", os.str());
        response.addHeader("Content-Length", std::to_string(body.size()));
        response.setBody(body);
    } else {
        size_t length_body = response.getBody().size();
        const char *content_type_body = response.getMimeType();
        std::ostringstream os;
        response.setCode(onyxup::ResponseState::RESPONSE_STATE_PARTIAL_CONTENT_CODE);
        response.setCodeMsg(onyxup::ResponseState::RESPONSE_STATE_PARTIAL_CONTENT_MSG);
        response.setMimeType(onyxup::MimeType::MIME_TYPE_MULTIPART_BYTES_RANGES);
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
        response.addHeader("Content-Length", std::to_string(response.getBody().size()));
    }
}

namespace onyxup {
    class ResponsePrepareRangeChain : public IResponsePrepareChain {
    public:
        virtual void execute(PtrTask task, onyxup::ResponseBase &response) override {
            if (checkRequestRange(task)) {
                try {
                    std::vector<std::pair<size_t, size_t>> ranges = utils::parseRangesRequest(
                            task->getRequest()->getHeaderRef("range"), response.getBody().size() - 1);
                    prepareRangeResponse(response, ranges);
                } catch (OnyxupException &ex) {
                    prepareRangeNotSatisfiableResponse(response);
                }
            } else {
                if (nextChain != nullptr)
                    nextChain->execute(task, response);
            }
        }
    };
}

