#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

namespace onyxup {
    
    class Request;
    
    using PtrRequest = Request *;
    using PtrCRequest = const Request * ;
    
    namespace request {
        PtrRequest factoryRequest();
        PtrRequest factoryCopyRequest(PtrRequest);
    }
    
    class Request {
        friend PtrRequest request::factoryRequest();
        friend PtrRequest request::factoryCopyRequest(PtrRequest);
    private:
        int m_fd;
        std::string m_full_uri;
        std::string m_uri;
        std::string m_method;
        std::string m_body;
        std::unordered_map<std::string, std::string> m_headers;
        std::unordered_map<std::string, std::string> m_params;
        
        bool m_header_accept;
        bool m_body_accept;
        bool m_body_exists;
        bool m_closing_connect;
        
        size_t m_max_output_length_buffer;
        
        Request(){};
    public:

        int getFD() const;

        void clear();
        
        inline void setFD(int fd) {
            m_fd = fd;
        }
        
        inline bool isHeaderAccept() const {
            return m_header_accept;
        }

        inline void setHeaderAccept(bool header_accept) {
            m_header_accept = header_accept;
        }
        
        inline void setFullURI(const char * uri, size_t size) {
            m_full_uri = std::string(uri, size);
        }
        
        inline void setMethod(const char * method, size_t size) {
            m_method = std::string(method, size);
        }

        inline void appendHeader(const std::string & key, const std::string & value){
            m_headers[key] = value;
        }
        
        inline void appendParam(const std::string & key, const std::string & value){
            m_params[key] = value;
        }
        
        inline const std::string & getFullURI() const {
            return m_full_uri;
        }
        
        inline std::string getMethod() const {
            return m_method;
        }
        
        inline bool isBodyAccept() const {
            return m_body_accept;
        }
        
        inline void setBodyAccept(bool body_accept) {
            m_body_accept = body_accept;
        }
        
        inline void setBody(const char* body, size_t size){
            m_body = std::string(body, size);
        }
        
        const std::string & getURIRef() const {
            return m_uri;
        }

        std::string getURI() const {
            return m_uri;
        }

        bool isBodyExists(){
            return m_body_exists;
        }

        void setURI(const std::string & uri) {
            m_uri = uri;
        }

        void setBodyExists(bool value){
           m_body_exists = value;
        }
        
        size_t getMaxOutputLengthBuffer() const {
            return m_max_output_length_buffer;
        }
        
        void setMaxOutputLengthBuffer(size_t max_output_length_buffer) {
            m_max_output_length_buffer = max_output_length_buffer;
        }
        
        const std::string & getBodyRef() const {
            return m_body;
        }

        std::string getBody() const {
            return m_body;
        }
        
        const std::unordered_map<std::string, std::string> getParams() const {
            return m_params;
        }
        
        const std::string & getHeaderRef(const std::string & key) const ;

        std::string getHeader(const std::string & key) const;

        bool isClosingConnect() const;

        void setClosingConnect(bool closing);


    };
    
}