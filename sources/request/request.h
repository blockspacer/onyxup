#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

namespace onyxup {
    
    class Request;
    
    using PtrRequest = Request *;
    using PtrCRequest = const Request * ;
    
    namespace req {
        PtrRequest requestFactory();
        PtrRequest requestCopyFactory(PtrRequest);
    }
    
    class Request {
        friend PtrRequest req::requestFactory();
        friend PtrRequest req::requestCopyFactory(PtrRequest);
    private:
        int fd;
        std::string fullUri;
        std::string uri;
        std::string method;
        std::string body;
        std::unordered_map<std::string, std::string> headers;
        std::unordered_map<std::string, std::string> params;
        
        bool headerAccept;
        bool bodyAccept;
        bool bodyExists;
        bool closingConnect;
        
        size_t maxOutputBufferLength;
        
        Request(){};
    public:

        int getFD() const;

        void clear();
        
        inline void setFD(int fd_) {
            fd = fd_;
        }
        
        inline bool isHeaderAccept() const {
            return headerAccept;
        }

        inline void setHeaderAccept(bool accept) {
            headerAccept = accept;
        }
        
        inline void setFullURI(const char * uri, size_t n) {
            fullUri = std::string(uri, n);
        }
        
        inline void setMethod(const char * method, size_t n) {
            this->method = std::string(method, n);
        }

        inline void addHeader(const std::string & key, const std::string & value){
            headers[key] = value;
        }
        
        inline void addParam(const std::string & key, const std::string & value){
            params[key] = value;
        }
        
        inline const std::string & getFullURIRef() const {
            return fullUri;
        }

        inline const std::string getFullURI() const {
            return fullUri;
        }
        
        inline std::string getMethod() const {
            return method;
        }
        
        inline bool isBodyAccept() const {
            return bodyAccept;
        }
        
        inline void setBodyAccept(bool accept) {
            bodyAccept = accept;
        }
        
        inline void setBody(const char* body, size_t n){
            this->body = std::string(body, n);
        }
        
        const std::string & getURIRef() const {
            return uri;
        }

        std::string getURI() const {
            return uri;
        }

        bool isBodyExists(){
            return bodyExists;
        }

        void setURI(const std::string & uri) {
            this->uri = uri;
        }

        void setBodyExists(bool value){
            bodyExists = value;
        }
        
        size_t getMaxOutputBufferLength() const {
            return maxOutputBufferLength;
        }
        
        void setMaxOutputLengthBuffer(size_t n) {
            maxOutputBufferLength = n;
        }
        
        const std::string & getBodyRef() const {
            return body;
        }

        std::string getBody() const {
            return body;
        }
        
        const std::unordered_map<std::string, std::string> getParams() const {
            return params;
        }
        
        const std::string & getHeaderRef(const std::string & key) const ;

        std::string getHeader(const std::string & key) const;

        bool isClosingConnect() const;

        void setClosingConnect(bool closing);


    };
    
}