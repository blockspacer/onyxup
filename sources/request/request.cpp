#include "request.h"

onyxup::PtrRequest onyxup::req::requestFactory() {
    PtrRequest requets = new (std::nothrow) Request;
    if (requets) {
        requets->closingConnect = false;
        requets->headerAccept = false;
        requets->bodyAccept = false;
        requets->bodyExists = false;
        return requets;
    }
    return nullptr;
}

void onyxup::Request::clear() {
    headerAccept = false;
    bodyAccept = false;
    bodyExists = false;

    fullUri.clear();
    uri.clear();
    method.clear();
    body.clear();
    headers.clear();
    params.clear();
}

onyxup::PtrRequest onyxup::req::requestCopyFactory(PtrRequest src) {
    PtrRequest dst = new (std::nothrow) Request;
    if (dst) {
        *dst = *src;
        return dst;
    }
    return nullptr;
}

const std::string & onyxup::Request::getHeaderRef(const std::string & key) const {
    return headers.at(key);
}

std::string onyxup::Request::getHeader(const std::string & key) const {
    return headers.at(key);
}

int onyxup::Request::getFD() const {
    return fd;
}

bool onyxup::Request::isClosingConnect() const {
    return closingConnect;
}

void onyxup::Request::setClosingConnect(bool closing) {
    closingConnect = closing;
}
