#pragma once

#include <string>
#include <sstream>
#include <unordered_map>
#include <chrono>
#include <time.h>

#include "../gzip/compress.hpp"
#include "../gzip/config.hpp"
#include "../gzip/decompress.hpp"
#include "../gzip/utils.hpp"
#include "../gzip/version.hpp"
#include "../version.h"
#include "../plog/Log.h"

namespace onyxup {
    
    class ResponseBase {
    private:

        std::string header;
        std::string body;
        int code;
        const char * codeMsg;
        const char * mimeType;
        bool compress;
        std::unordered_map<std::string, std::string> m_headers;

        std::string prepareResponse();

        static constexpr const char *HEADER = "HTTP/1.1 %d %s\r\nHost: %s:%d\r\nServer: onyxup/%s\r\nConnection: Keep-Alive\r\nDate: %s\r\nAccept-Ranges: bytes\r\nContent-Type: %s%s\r\n\r\n";

    public:
        
        static std::string SERVER_IP;
        static int SERVER_PORT;

        ResponseBase(int code, const char *codeMsg, const char *mime, const std::string &body, bool compress = false) : code(code), codeMsg(codeMsg), mimeType(mime), body(body), compress(compress){
        }

        ResponseBase(int code, const char *codeMsg, const char *mime, bool compress = false): ResponseBase(code, codeMsg, mime, "", compress){
        }

        ResponseBase(){
        }

        ResponseBase(const ResponseBase &) = default;
        ResponseBase & operator=(const ResponseBase &) = default;

        ResponseBase(ResponseBase &&);
        ResponseBase & operator=(ResponseBase &&);

        operator std::string() {
            return prepareResponse();
        }

        std::string toString() {
            return prepareResponse();
        }

        const char *getMimeType() const;

        void setBody(const std::string &body);

        void addHeader(const std::string &key, const std::string &value);

        const std::string & getBody() const;

        bool isCompress() const;

        void setCode(int code);

        void setCodeMsg(const char *codeMsg);

        void setMimeType(const char *mimeType);

        int getCode() const;

    };
}
