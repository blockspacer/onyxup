#include "response-base.h"

std::string onyxup::ResponseBase::prepareResponse() {
    char buffer [4096];
    std::ostringstream os;

    for(auto header : m_headers)
        os << "\r\n" << header.first << ": " << header.second;

    struct tm tm;
    std::chrono::time_point<std::chrono::system_clock> date =  std::chrono::system_clock::now();
    std::time_t  time = std::chrono::system_clock::to_time_t(date);
    memset(&tm, 0, sizeof(tm));
    localtime_r(&time, &tm);
    char datetime[80];
    memset(datetime, '\0', sizeof (datetime));
    strftime(datetime, sizeof (datetime), "%Y-%m-%d %H:%M:%S", &tm);

    snprintf(buffer, sizeof(buffer), ResponseBase::HEADER, m_code, m_code_msg, VERSION_APPLICATION, datetime, m_mime, os.str().c_str());
    m_header = std::string(buffer);
    return m_header + m_body;
}

onyxup::ResponseBase::ResponseBase(onyxup::ResponseBase && src) {
    m_code = src.m_code;
    m_mime = src.m_mime;
    m_code_msg = src.m_code_msg;
    m_compress = src.m_compress;
    m_header = std::move(src.m_header);
    m_body = std::move(src.m_body);
    m_header = std::move(src.m_header);
}

onyxup::ResponseBase& onyxup::ResponseBase::operator=(onyxup::ResponseBase && src) {
    if (&src == this)
        return *this;
    delete this;
    m_code = src.m_code;
    m_mime = src.m_mime;
    m_code_msg = src.m_code_msg;
    m_compress = src.m_compress;
    m_header = std::move(src.m_header);
    m_body = std::move(src.m_body);
    m_header = std::move(src.m_header);
    return *this;
}

void onyxup::ResponseBase::setBody(const std::string &body) {
    m_body = body;
}

void onyxup::ResponseBase::appendHeader(const std::string &key, const std::string &value) {
    m_headers[key] = value;
}

const std::string & onyxup::ResponseBase::getBody() const {
    return m_body;
}

bool onyxup::ResponseBase::isCompress() const {
    return m_compress;
}

void onyxup::ResponseBase::setCode(int code) {
    m_code = code;
}

void onyxup::ResponseBase::setCodeMsg(const char *codeMsg) {
    m_code_msg = codeMsg;
}

void onyxup::ResponseBase::setMime(const char *mime) {
    m_mime = mime;
}

const char *onyxup::ResponseBase::getMime() const {
    return m_mime;
}

int onyxup::ResponseBase::getCode() const {
    return m_code;
}
