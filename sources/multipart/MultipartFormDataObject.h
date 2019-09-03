#pragma once

#include <string>

namespace onyxup {
    class MultipartFormDataObject {
    private:
        std::vector<char> m_data;
        std::string m_name;
        std::string m_filename;
        std::string m_content_type;

    public:

        MultipartFormDataObject() = default;

        MultipartFormDataObject(const std::string &name, const std::string &filename, const std::string &content_type, const std::vector<char> &data) :
        m_name(name),
        m_filename(filename),
        m_content_type(content_type),
        m_data(data) {}

        const std::string getContentType() const {
            return m_content_type;
        }

        const std::vector<char> getData() const {
            return m_data;
        }

        const std::vector<char> & getDataReference() const {
            return m_data;
        }

        const std::string getName() const {
            return m_name;
        }

        const std::string getFilename() const {
            return m_filename;
        }

    };
}