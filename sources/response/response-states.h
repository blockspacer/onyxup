#pragma once

namespace onyxup {

    class ResponseState {
    public:
        static constexpr const int RESPONSE_STATE_CONTINUE_CODE = 100;
        static constexpr const char * RESPONSE_STATE_CONTINUE_MSG = "Continue";

        static constexpr const int RESPONSE_STATE_SWITCHING_PROTOCOLS_CODE = 101;
        static constexpr const char * RESPONSE_STATE_SWITCHING_PROTOCOLS_MSG = "Switching protocols";
        
        static constexpr const int RESPONSE_STATE_PROCESSING_CODE = 102;
        static constexpr const char * RESPONSE_STATE_PROCESSING_MSG = "Processing";
        
        static constexpr const int RESPONSE_STATE_OK_CODE = 200;
        static constexpr const char * RESPONSE_STATE_OK_MSG = "OK";
        
        static constexpr const int RESPONSE_STATE_CREATED_CODE = 201;
        static constexpr const char * RESPONSE_STATE_CREATED_MSG = "Created";
        
        static constexpr const int RESPONSE_STATE_ACCEPTED_CODE = 202;
        static constexpr const char * RESPONSE_STATE_ACCEPTED_MSG = "Accepted";
        
        static constexpr const int RESPONSE_STATE_NON_CODE = 203;
        static constexpr const char * RESPONSE_STATE_NON_MSG = "Authoritative Information";
        
        static constexpr const int RESPONSE_STATE_NO_CONTENT_CODE = 204;
        static constexpr const char * RESPONSE_STATE_NO_CONTENT_MSG = "No Content";
        
        static constexpr const int RESPONSE_STATE_RESET_CONTENT_CODE = 205;
        static constexpr const char * RESPONSE_STATE_RESET_CONTENT_MSG = "Reset Content";
        
        static constexpr const int RESPONSE_STATE_PARTIAL_CONTENT_CODE = 206;
        static constexpr const char * RESPONSE_STATE_PARTIAL_CONTENT_MSG = "Partial Content";
        
        static constexpr const int RESPONSE_STATE_MULTI_STATUS_CODE = 207;
        static constexpr const char * RESPONSE_STATE_MULTI_STATUS_MSG = "Multi - Status";
        
        static constexpr const int RESPONSE_STATE_ALREADY_REPORTED_CODE = 208;
        static constexpr const char * RESPONSE_STATE_ALREADY_REPORTED_MSG = "Already Reported";
        
        static constexpr const int RESPONSE_STATE_IM_USED_CODE = 226;
        static constexpr const char * RESPONSE_STATE_IM_USED_MSG = "IM Used";
        
        static constexpr const int RESPONSE_STATE_MULTIPLE_CHOICES_CODE = 300;
        static constexpr const char * RESPONSE_STATE_MULTIPLE_CHOICES_MSG = "Multiple Choices";
        
        static constexpr const int RESPONSE_STATE_MOVED_PERMANENTLY_CODE = 301;
        static constexpr const char * RESPONSE_STATE_MOVED_PERMANENTLY_MSG = "Moved Permanently";
        
        static constexpr const int RESPONSE_STATE_MOVED_TEMPORARILY_CODE = 302;
        static constexpr const char * RESPONSE_STATE_MOVED_TEMPORARILY_MSG = "Moved Temporarily";
        
        static constexpr const int RESPONSE_STATE_FOUND_CODE = 302;
        static constexpr const char * RESPONSE_STATE_FOUND_MSG = "Found";
        
        static constexpr const int RESPONSE_STATE_SEE_OTHER_CODE = 303;
        static constexpr const char * RESPONSE_STATE_SEE_OTHER_MSG = "See Other";
        
        static constexpr const int RESPONSE_STATE_NOT_MODIFIED_CODE = 304;
        static constexpr const char * RESPONSE_STATE_NOT_MODIFIED_MSG = "Not Modified";
        
        static constexpr const int RESPONSE_STATE_USE_PROXY_CODE = 305;
        static constexpr const char * RESPONSE_STATE_USE_PROXY_MSG = "Use Proxy";
        
        static constexpr const int RESPONSE_STATE_TEMPORARY_REDIRECT_CODE = 307;
        static constexpr const char * RESPONSE_STATE_TEMPORARY_REDIRECT_MSG = "Temporary Redirect";
        
        static constexpr const int RESPONSE_STATE_PERMANENT_REDIRECT_CODE = 308;
        static constexpr const char * RESPONSE_STATE_PERMANENT_REDIRECT_MSG = "Permanent Redirect";
        
        static constexpr const int RESPONSE_STATE_BAD_REQUEST_CODE = 400;
        static constexpr const char * RESPONSE_STATE_BAD_REQUEST_MSG = "Bad Request";
        
        static constexpr const int RESPONSE_STATE_UNAUTHORIZED_CODE = 401;
        static constexpr const char * RESPONSE_STATE_UNAUTHORIZED_MSG = "Unauthorized";
        
        static constexpr const int RESPONSE_STATE_PAYMENT_REQUIRED_CODE = 402;
        static constexpr const char * RESPONSE_STATE_PAYMENT_REQUIRED_MSG = "Payment Required";
        
        static constexpr const int RESPONSE_STATE_FORBIDDEN_CODE = 403;
        static constexpr const char * RESPONSE_STATE_FORBIDDEN_MSG = "Forbidden";
        
        static constexpr const int RESPONSE_STATE_NOT_FOUND_CODE = 404;
        static constexpr const char * RESPONSE_STATE_NOT_FOUND_MSG = "Not Found";
        
        static constexpr const int RESPONSE_STATE_METHOD_NOT_ALLOWED_CODE = 405;
        static constexpr const char * RESPONSE_STATE_METHOD_NOT_ALLOWED_MSG = "Method Not Allowed";
        
        static constexpr const int RESPONSE_STATE_METHOD_NOT_ACCEPTABLE_CODE = 406;
        static constexpr const char * RESPONSE_STATE_METHOD_NOT_ACCEPTABLE_MSG = "Not Acceptable";
        
        static constexpr const int RESPONSE_STATE_METHOD_PROXY_AUTHENTICATION_REQUIRED_CODE = 407;
        static constexpr const char * RESPONSE_STATE_METHOD_PROXY_AUTHENTICATION_REQUIRED_MSG = "Proxy Authentication Required";
        
        static constexpr const int RESPONSE_STATE_METHOD_REQUEST_TIMEOUT_CODE = 408;
        static constexpr const char * RESPONSE_STATE_METHOD_REQUEST_TIMEOUT_MSG = "Request Timeout";
        
        static constexpr const int RESPONSE_STATE_METHOD_CONFLICT_CODE = 409;
        static constexpr const char * RESPONSE_STATE_METHOD_CONFLICT_MSG = "Conflict";
        
        static constexpr const int RESPONSE_STATE_METHOD_GONE_CODE = 410;
        static constexpr const char * RESPONSE_STATE_METHOD_GONE_MSG = "Gone";
        
        static constexpr const int RESPONSE_STATE_LENGTH_REQUIRED_CODE = 411;
        static constexpr const char * RESPONSE_STATE_METHOD_LENGTH_REQUIRED_MSG = "Length Required";
        
        static constexpr const int RESPONSE_STATE_PRECONDITION_FAILED_CODE = 412;
        static constexpr const char * RESPONSE_STATE_METHOD_PRECONDITION_FAILED_MSG = "Precondition Failed";
        
        static constexpr const int RESPONSE_STATE_PAYLOAD_TOO_LARGE_CODE = 413;
        static constexpr const char * RESPONSE_STATE_METHOD_PAYLOAD_TOO_LARGE_MSG = "Payload Too Large";
        
        static constexpr const int RESPONSE_STATE_URI_TOO_LONG_CODE = 414;
        static constexpr const char * RESPONSE_STATE_METHOD_URI_TOO_LONG_MSG = "URI Too Long";
        
        static constexpr const int RESPONSE_STATE_UNSUPPORTED_MEDIA_TYPE_CODE = 415;
        static constexpr const char * RESPONSE_STATE_UNSUPPORTED_MEDIA_TYPE_MSG = "Unsupported Media Type";
        
        static constexpr const int RESPONSE_STATE_RANGE_NOT_SATISFIABLE_CODE = 416;
        static constexpr const char * RESPONSE_STATE_RANGE_NOT_SATISFIABLE_MSG = "Range Not Satisfiable";
        
        static constexpr const int RESPONSE_STATE_EXPECTATION_FAILED_CODE = 417;
        static constexpr const char * RESPONSE_STATE_EXPECTATION_FAILED_MSG = "Expectation Failed";
        
        static constexpr const int RESPONSE_STATE_IM_TEAPOT_CODE = 418;
        static constexpr const char * RESPONSE_STATE_IM_TEAPOT_MSG = "I’m a teapot";
        
        static constexpr const int RESPONSE_STATE_AUTHENTICATION_TIMEOUT_CODE = 419;
        static constexpr const char * RESPONSE_STATE_AUTHENTICATION_TIMEOUT_MSG = "Authentication Timeout";
        
        static constexpr const int RESPONSE_STATE_MISDIRECTED_REQUEST_CODE = 421;
        static constexpr const char * RESPONSE_STATE_MISDIRECTED_REQUEST_MSG = "Misdirected Request";
        
        static constexpr const int RESPONSE_STATE_UNPROCESSABLE_ENTITY_CODE = 422;
        static constexpr const char * RESPONSE_STATE_UNPROCESSABLE_ENTITY_MSG = "Unprocessable Entity";
        
        static constexpr const int RESPONSE_STATE_LOCKED_CODE = 423;
        static constexpr const char * RESPONSE_STATE_LOCKED_MSG = "Locked";
        
        static constexpr const int RESPONSE_STATE_FAILED_DEPENDENCY_CODE = 424;
        static constexpr const char * RESPONSE_STATE_FAILED_DEPENDENCY_MSG = "Failed Dependency";
        
        static constexpr const int RESPONSE_STATE_UPGRADE_REQUIRED_CODE = 426;
        static constexpr const char * RESPONSE_STATE_UPGRADE_REQUIRED_MSG = "Upgrade Required";
        
        static constexpr const int RESPONSE_STATE_PRECONDITION_REQUIRED_CODE = 428;
        static constexpr const char * RESPONSE_STATE_PRECONDITION_REQUIRED_MSG = "Precondition Required";
        
        static constexpr const int RESPONSE_STATE_TOO_MANY_REQUEST_CODE = 429;
        static constexpr const char * RESPONSE_STATE_TOO_MANY_REQUEST_MSG = "Too Many Requests";
        
        static constexpr const int RESPONSE_STATE_REQUEST_HEADER_FIELDS_TOO_LARGE_CODE = 431;
        static constexpr const char * RESPONSE_STATE_REQUEST_HEADER_FIELDS_TOO_LARGE_MSG = "Request Header Fields Too Large";
        
        static constexpr const int RESPONSE_STATE_RETRY_WITH_CODE = 449;
        static constexpr const char * RESPONSE_STATE_RETRY_WITH_MSG = "Retry With";
        
        static constexpr const int RESPONSE_STATE_UNAVAILABLE_FOR_LEGAL_REASONS_CODE = 451;
        static constexpr const char * RESPONSE_STATE_UNAVAILABLE_FOR_LEGAL_REASONS_MSG = "Unavailable For Legal Reasons";
        
        static constexpr const int RESPONSE_STATE_CLIENT_CLOSED_REQUEST_CODE = 499;
        static constexpr const char * RESPONSE_STATE_CLIENT_CLOSED_REQUEST_MSG = "Client Closed Request";
        
        static constexpr const int RESPONSE_STATE_INTERNAL_SERVER_ERROR_CODE = 500;
        static constexpr const char * RESPONSE_STATE_INTERNAL_SERVER_ERROR_MSG = "Internal Server Error";
        
        static constexpr const int RESPONSE_STATE_NOT_IMPLEMENTED_CODE = 501;
        static constexpr const char * RESPONSE_STATE_NOT_IMPLEMENTED_MSG = "Not Implemented";
        
        static constexpr const int RESPONSE_STATE_BAD_GATEWAY_CODE = 502;
        static constexpr const char * RESPONSE_STATE_BAD_GATEWAY_MSG = "Bad Gateway";
        
        static constexpr const int RESPONSE_STATE_SERVICE_UNAVAILABLE_CODE = 503;
        static constexpr const char * RESPONSE_STATE_SERVICE_UNAVAILABLE_MSG = "Service Unavailable";
        
        static constexpr const int RESPONSE_STATE_GATEWAY_TIMEOUT_CODE = 504;
        static constexpr const char * RESPONSE_STATE_GATEWAY_TIMEOUT_MSG = "Gateway Timeout";
        
        static constexpr const int RESPONSE_STATE_HTTP_VERSION_NOT_SUPPORTED_CODE = 505;
        static constexpr const char * RESPONSE_STATE_HTTP_VERSION_NOT_SUPPORTED_MSG = "HTTP Version Not Supported";
        
        static constexpr const int RESPONSE_STATE_VARIANT_ALSO_NEGOTIATES_CODE = 506;
        static constexpr const char * RESPONSE_STATE_VARIANT_ALSO_NEGOTIATES_MSG = "Variant Also Negotiates";
        
        static constexpr const int RESPONSE_STATE_INSUFFICIENT_STORAGE_CODE = 507;
        static constexpr const char * RESPONSE_STATE_INSUFFICIENT_STORAGE_MSG = "Insufficient Storage";
        
        static constexpr const int RESPONSE_STATE_LOOP_DETECTED_CODE = 508;
        static constexpr const char * RESPONSE_STATE_LOOP_DETECTED_MSG = "Loop Detected";
        
        static constexpr const int RESPONSE_STATE_BANDWIDTH_LIMIT_EXCEEDED_CODE = 509;
        static constexpr const char * RESPONSE_STATE_BANDWIDTH_LIMIT_EXCEEDED_MSG = "Bandwidth Limit Exceeded";
        
        static constexpr const int RESPONSE_STATE_NOT_EXTENDED_CODE = 510;
        static constexpr const char * RESPONSE_STATE_NOT_EXTENDED_MSG = "Not Extended";
        
        static constexpr const int RESPONSE_STATE_NETWORK_AUTHENTICATION_REQUIRED_CODE = 511;
        static constexpr const char * RESPONSE_STATE_NETWORK_AUTHENTICATION_REQUIRED_MSG = "Network Authentication Required";
        
        static constexpr const int RESPONSE_STATE_UNKNOWN_ERROR_CODE = 520;
        static constexpr const char * RESPONSE_STATE_UNKNOWN_ERROR_MSG = "Unknown Error";
        
        static constexpr const int RESPONSE_STATE_WEB_SERVER_IS_DOWN_CODE = 521;
        static constexpr const char * RESPONSE_STATE_WEB_SERVER_IS_DOWN_MSG = "Web Server Is Down";
        
        static constexpr const int RESPONSE_STATE_CONNECTION_TIMEOUT_CODE = 522;
        static constexpr const char * RESPONSE_STATE_CONNECTION_TIMEOUT_MSG = "Connection Timed Out";
        
        static constexpr const int RESPONSE_STATE_ORIGIN_IS_UNREACHABLE_CODE = 523;
        static constexpr const char * RESPONSE_STATE_ORIGIN_IS_UNREACHABLE_MSG = "Origin Is Unreachable";
        
        static constexpr const int RESPONSE_STATE_TIMEOUT_OCCURRED_CODE = 524;
        static constexpr const char * RESPONSE_STATE_TIMEOUT_OCCURRED_MSG = "A Timeout Occurred";
        
        static constexpr const int RESPONSE_STATE_SSL_HANDSHAKE_FAILED_CODE = 525;
        static constexpr const char * RESPONSE_STATE_SSL_HANDSHAKE_FAILED_MSG = "SSL Handshake Failed";
        
        static constexpr const int RESPONSE_STATE_INVALID_SSL_CERTIFICATE_CODE = 526;
        static constexpr const char * RESPONSE_STATE_INVALID_SSL_CERTIFICATE_MSG = "Invalid SSL Certificate";
    };
}