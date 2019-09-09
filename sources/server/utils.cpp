#include "utils.h"

static std::string trimCharacters(const std::vector<char> &src, std::initializer_list<char> chars) {
    auto start = src.begin(), end = src.end() - 1;
    while (start != end) {
        bool exists = false;
        for (auto &ch : chars) {
            if (*start == ch) {
                exists = true;
                break;
            }
        }
        if (exists)
            start++;
        else
            break;
    }
    while (end != start) {
        bool exists = false;
        for (auto &ch : chars) {
            if (*end == ch) {
                exists = true;
                break;
            }
        }
        if (exists)
            end--;
        else
            break;
    }
    return std::string(start, end + 1);
}

static std::string trimSpaces(const std::string &src) {
    std::string dst;
    for (size_t i = 0; i < src.size(); i++)
        if (src[i] != ' ')
            dst += src[i];
    return dst;
}

std::unordered_map<std::string, std::string> onyxup::utils::urlencoded(const std::string &src) {
    std::unordered_map<std::string, std::string> params;
    std::istringstream iss(src);
    std::string token;
    while (std::getline(iss, token, '&')) {
        size_t pos = token.find("=");
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1, token.size());
            if (!key.empty() && !value.empty())
                params[key] = value;
        }
    }
    return params;
}

std::unordered_map<std::string, onyxup::MultipartFormDataObject> onyxup::utils::multipartFormData(PtrCRequest request) {
    std::unordered_map<std::string, MultipartFormDataObject> fields;
    std::string boundary;
    try {
        std::string content_type = request->getHeaderRef("content-type");
        size_t pos = content_type.find("boundary=");
        if (content_type.find("multipart/form-data") == std::string::npos || pos == std::string::npos)
            return fields;
        boundary = content_type.substr(pos + strlen("boundary="));
        if (boundary.empty())
            return fields;
    } catch (std::out_of_range) {
        return fields;
    }
    size_t pos = request->getBodyRef().find(boundary, 0);
    while (pos != std::string::npos && pos < request->getBodyRef().size()) {
        size_t position_content_disposition = request->getBodyRef().find("Content-Disposition: form-data",
                                                                         pos + boundary.length());
        if (position_content_disposition == std::string::npos)
            break;
        std::vector<char> characters;
        std::string name;
        size_t position_name = request->getBodyRef().find("name=", pos + boundary.length());
        if (position_name != std::string::npos) {
            position_name += strlen("name=");
            while (position_name < request->getBodyRef().length() &&
                   !isspace(request->getBodyRef()[position_name]) &&
                   request->getBodyRef()[position_name] != ';') {
                characters.push_back(request->getBodyRef()[position_name]);
                position_name++;
            }
            if (!characters.empty())
                name = trimCharacters(characters, {'"', ' '});
        }
        characters.clear();
        std::string file_name;
        size_t position_filename = request->getBodyRef().find("filename=", pos + boundary.length());
        if (position_filename != std::string::npos) {
            position_filename += strlen("filename=");
            while (position_filename < request->getBodyRef().length() &&
                   !isspace(request->getBodyRef()[position_filename]) &&
                   request->getBodyRef()[position_filename] != ';') {
                characters.push_back(request->getBodyRef()[position_filename]);
                position_filename++;
            }
            if (!characters.empty())
                file_name = trimCharacters(characters, {'"', ' '});
        }
        characters.clear();
        std::string content_type;
        size_t position_content_type = request->getBodyRef().find("Content-Type:", pos + boundary.length());
        if (position_content_type != std::string::npos) {
            position_content_type += strlen("Content-Type:");
            while (position_content_type < request->getBodyRef().length() &&
                   request->getBodyRef()[position_content_type] != '\n' &&
                   request->getBodyRef()[position_content_type] != '\r') {
                characters.push_back(request->getBodyRef()[position_content_type]);
                position_content_type++;
            }
            if (!characters.empty())
                content_type = trimCharacters(characters, {'"', ' '});
        }
        size_t position_data = request->getBodyRef().find("\r\n\r\n", pos + boundary.length());
        std::vector<char> data;
        pos = request->getBodyRef().find(boundary, pos + boundary.length());
        if (position_data != std::string::npos && pos != std::string::npos) {
            std::copy(request->getBodyRef().begin() + position_data + strlen("\r\n\r\n"),
                      request->getBodyRef().begin() + pos - strlen("\r\n"), std::back_inserter(data));
        }
        if (!name.empty())
            fields.emplace(std::piecewise_construct, std::make_tuple(name),
                           std::make_tuple(name, file_name, content_type, data));
    }
    return fields;
}

std::vector<std::pair<size_t, size_t>>
onyxup::utils::parseRangesRequest(const std::string &src, size_t length) {
    std::vector<std::pair<size_t, size_t>> ranges;
    if (!length)
        throw OnyxupException("Неверный формат HTTP range requests");

    int pos = src.find("bytes=");
    if (pos != std::string::npos) {
        std::istringstream iss(src.substr(pos + strlen("bytes=")));
        std::string token;
        while (std::getline(iss, token, ',')) {
            std::string token_with_out_spaces(std::move(trimSpaces(token)));
            if (token_with_out_spaces[token_with_out_spaces.size() - 1] == '-') {
                size_t value = 0;
                for (size_t i = 0; i < token_with_out_spaces.size() - 1; i++) {
                    if (!std::isdigit(token_with_out_spaces[i]))
                        throw onyxup::OnyxupException("Неверный формат HTTP range requests");
                    value = value * 10 + (token_with_out_spaces[i] - '0');
                }
                if (value > length)
                    throw OnyxupException("Неверный формат HTTP range requests");
                ranges.emplace_back(value, length);
            } else if (token_with_out_spaces[0] == '-') {
                size_t value = 0;
                for (size_t i = 1; i < token_with_out_spaces.size(); i++) {
                    if (!std::isdigit(token_with_out_spaces[i]))
                        throw OnyxupException("Неверный формат HTTP range requests");
                    value = value * 10 + (token_with_out_spaces[i] - '0');
                }
                if (value > length)
                    throw OnyxupException("Неверный формат HTTP range requests");
                ranges.emplace_back(length - value, length);
            } else {
                size_t pos = token_with_out_spaces.find("-");
                if (pos == std::string::npos)
                    throw OnyxupException("Неверный формат HTTP range requests");

                size_t value_left = 0;
                for (size_t i = 0; i < pos; i++) {
                    if (!std::isdigit(token_with_out_spaces[i]))
                        throw OnyxupException("Неверный формат HTTP range requests");
                    value_left = value_left * 10 + (token_with_out_spaces[i] - '0');
                }
                size_t value_right = 0;
                for (size_t i = pos + 1; i < token_with_out_spaces.size(); i++) {
                    if (!std::isdigit(token_with_out_spaces[i]))
                        throw OnyxupException("Неверный формат HTTP range requests");
                    value_right = value_right * 10 + (token_with_out_spaces[i] - '0');
                }
                if (value_right < value_left || value_right > length)
                    throw OnyxupException("Неверный формат HTTP range requests");
                ranges.emplace_back(value_left, value_right);
            }
        }
        if (ranges.empty())
            throw OnyxupException("Неверный формат HTTP range requests");
        std::sort(ranges.begin(), ranges.end(), [](auto &first, auto &second) -> bool {
            return first.first < second.first;
        });
        ssize_t border = 0;
        for (size_t i = 0; i < ranges.size(); i++) {
            std::pair<size_t, size_t> &pair = ranges[i];
            if (i) {
                if (pair.first < border || pair.second > length)
                    throw OnyxupException("Неверный формат HTTP range requests");
            }
            border = pair.second;
        }
        return ranges;
    }
    throw OnyxupException("Неверный формат HTTP range requests");
}

void onyxup::utils::parseParamsRequest(onyxup::PtrRequest request, size_t uri_len) {
    const char *full_uri = request->getFullURIRef().c_str();
    char *start_params_part = const_cast<char *>(strchr(full_uri, '?'));
    if (start_params_part) {
        request->setURI(std::string(full_uri, start_params_part - full_uri));
        size_t buf_params_len = uri_len - (start_params_part - full_uri);
        char *buf_params = new(std::nothrow) char[buf_params_len];
        if (buf_params) {
            memcpy(buf_params, start_params_part + 1, buf_params_len);
            char *save_ptr;
            char *token = strtok_r(buf_params, "&", &save_ptr);
            while (token != NULL) {
                char *sep = strchr(token, '=');
                if (sep) {
                    std::string key(token, sep - token);
                    std::string value(sep + 1, strlen(token) - (sep - token) - 1);
                    request->addParam(key, value);
                }
                token = strtok_r(NULL, "&", &save_ptr);
            }
            delete[] buf_params;
        }
    } else
        request->setURI(request->getFullURIRef());
}
