#pragma once

#include <unordered_map>
#include <string>
#include <sstream>
#include <string.h>
#include <vector>
#include <algorithm>
#include "../multipart/MultipartFormDataObject.h"
#include "../request/request.h"
#include "../exception/exception.h"

namespace onyxup {
    namespace utils {
        std::unordered_map<std::string, std::string> urlencoded(const std::string & src);
        std::unordered_map<std::string, MultipartFormDataObject> multipartFormData(PtrCRequest request);
        std::vector<std::pair<size_t , size_t>> parseRangesRequest(const std::string & src, size_t length);
        void parseParamsRequest(onyxup::PtrRequest request, size_t uri_len);
    }
}
