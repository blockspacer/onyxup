#include "request.h"

onyxup::PtrRequest onyxup::request::factoryRequest() {
    PtrRequest requets = new (std::nothrow) Request;
    if (requets) {
        requets->m_closing_connect = false;
        requets->m_header_accept = false;
        requets->m_body_accept = false;
        requets->m_body_exists = false;
        return requets;
    }
    return nullptr;
}

void onyxup::Request::clear() {
    m_header_accept = false;
    m_body_accept = false;
    m_body_exists = false;

    m_full_uri.clear();
    m_uri.clear();
    m_method.clear();
    m_body.clear();
    m_headers.clear();
    m_params.clear();
}

onyxup::PtrRequest onyxup::request::factoryCopyRequest(PtrRequest src) {
    PtrRequest dst = new (std::nothrow) Request;
    if (dst) {
        *dst = *src;
        return dst;
    }
    return nullptr;
}

const std::string & onyxup::Request::getHeaderRef(const std::string & key) const {
    return m_headers.at(key);
}

std::string onyxup::Request::getHeader(const std::string & key) const {
    return m_headers.at(key);
}

int onyxup::Request::getFD() const {
    return m_fd;
}

bool onyxup::Request::isClosingConnect() const {
    return m_closing_connect;
}

void onyxup::Request::setClosingConnect(bool closing) {
    m_closing_connect = closing;
}
