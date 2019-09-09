#include "response-base.h"

std::string onyxup::ResponseBase::SERVER_IP = "";
int onyxup::ResponseBase::SERVER_PORT = 80;

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

    snprintf(buffer, sizeof(buffer), ResponseBase::HEADER, code, codeMsg, SERVER_IP.c_str(), SERVER_PORT, VERSION_APPLICATION, datetime, mimeType, os.str().c_str());
    header = std::string(buffer);
    return header + body;
}

onyxup::ResponseBase::ResponseBase(onyxup::ResponseBase && src) {
    code = src.code;
    mimeType = src.mimeType;
    codeMsg = src.codeMsg;
    compress = src.compress;
    header = std::move(src.header);
    body = std::move(src.body);
    header = std::move(src.header);
}

onyxup::ResponseBase& onyxup::ResponseBase::operator=(onyxup::ResponseBase && src) {
    if (&src == this)
        return *this;
    delete this;
    code = src.code;
    mimeType = src.mimeType;
    codeMsg = src.codeMsg;
    compress = src.compress;
    header = std::move(src.header);
    body = std::move(src.body);
    header = std::move(src.header);
    return *this;
}

void onyxup::ResponseBase::setBody(const std::string &body) {
    this->body = body;
}

void onyxup::ResponseBase::addHeader(const std::string &key, const std::string &value) {
    m_headers[key] = value;
}

const std::string & onyxup::ResponseBase::getBody() const {
    return body;
}

bool onyxup::ResponseBase::isCompress() const {
    return compress;
}

void onyxup::ResponseBase::setCode(int code) {
    this->code = code;
}

void onyxup::ResponseBase::setCodeMsg(const char *codeMsg) {
    this->codeMsg = codeMsg;
}

void onyxup::ResponseBase::setMimeType(const char *mimeType) {
    this->mimeType = mimeType;
}

const char *onyxup::ResponseBase::getMimeType() const {
    return mimeType;
}

int onyxup::ResponseBase::getCode() const {
    return code;
}
