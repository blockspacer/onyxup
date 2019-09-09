#pragma once

#include <string>

namespace onyxup {
    class MultipartFormDataObject {
    private:
        std::vector<char> data;
        std::string name;
        std::string filename;
        std::string contentType;

    public:

        MultipartFormDataObject() = default;

        MultipartFormDataObject(const std::string &name, const std::string &filename, const std::string &contentType, const std::vector<char> &data) :
                name(name),
                filename(filename),
                contentType(contentType),
                data(data) {}

        const std::string getContentType() const {
            return contentType;
        }

        const std::vector<char> getData() const {
            return data;
        }

        const std::vector<char> & getDataRef() const {
            return data;
        }

        const std::string getName() const {
            return name;
        }

        const std::string getFilename() const {
            return filename;
        }

    };
}