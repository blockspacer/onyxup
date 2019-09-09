#pragma once

#include <unordered_map>

namespace onyxup {

    class MimeType {
    public:
        static constexpr const char * MIME_TYPE_APPLICATION_ATOM = "application/atom+xml";
        static constexpr const char * MIME_TYPE_APPLICATION_EDI_X12 = "application/EDI-X12";
        static constexpr const char * MIME_TYPE_APPLICATION_EDIFACT = "application/EDIFACT";
        static constexpr const char * MIME_TYPE_APPLICATION_JSON = "application/json; charset=utf-8";
        static constexpr const char * MIME_TYPE_APPLICATION_JAVASCRIPT = "application/javascript";
        static constexpr const char * MIME_TYPE_APPLICATION_OCTET_STREAM = "application/octet-stream";
        static constexpr const char * MIME_TYPE_APPLICATION_OGG = "application/ogg";
        static constexpr const char * MIME_TYPE_APPLICATION_PDF = "application/pdf; charset=utf-8";
        static constexpr const char * MIME_TYPE_APPLICATION_POSTSCRIPT = "application/postscript";
        static constexpr const char * MIME_TYPE_APPLICATION_SOAP = "application/soap+xml";
        static constexpr const char * MIME_TYPE_APPLICATION_FONT_WOFF = "application/font-woff";
        static constexpr const char * MIME_TYPE_APPLICATION_XHTML = "application/xhtml+xml";
        static constexpr const char * MIME_TYPE_APPLICATION_DTD = "application/xml-dtd";
        static constexpr const char * MIME_TYPE_APPLICATION_XOP = "application/xop+xml";
        static constexpr const char * MIME_TYPE_APPLICATION_ZIP = "application/zip";
        static constexpr const char * MIME_TYPE_APPLICATION_GZIP = "application/gzip";
        static constexpr const char * MIME_TYPE_APPLICATION_BITTORRENT = "application/x-bittorrent";
        static constexpr const char * MIME_TYPE_APPLICATION_TEX = "application/x-tex";
        static constexpr const char * MIME_TYPE_APPLICATION_XML = "application/xml; charset=utf-8";

        /*
         * Audio
         */

        static constexpr const char * MIME_TYPE_AUDIO_MULAW = "audio/basic";
        static constexpr const char * MIME_TYPE_AUDIO_LINEAR_PCM = "audio/L24";
        static constexpr const char * MIME_TYPE_AUDIO_MP4 = "audio/mp4";
        static constexpr const char * MIME_TYPE_AUDIO_AAC = "audio/aac";
        static constexpr const char * MIME_TYPE_AUDIO_MP3 = "audio/mpeg";
        static constexpr const char * MIME_TYPE_AUDIO_OGG = "audio/ogg";
        static constexpr const char * MIME_TYPE_AUDIO_VORBIS = "audio/vorbis";
        static constexpr const char * MIME_TYPE_AUDIO_WINDOWS_MEDIA_AUDIO = "audio/x-ms-wma";
        static constexpr const char * MIME_TYPE_AUDIO_WINDOWS_MEDIA_AUDIO_2 = "audio/x-ms-wax";
        static constexpr const char * MIME_TYPE_AUDIO_REAL_AUDO = "audio/vnd.rn-realaudio";
        static constexpr const char * MIME_TYPE_AUDIO_WAV = "audio/vnd.wave";
        static constexpr const char * MIME_TYPE_AUDIO_WEBM = "audio/webm";

        /*
         * Image
         */
        static constexpr const char * MIME_TYPE_IMAGE_GIF = "image/gif";
        static constexpr const char * MIME_TYPE_IMAGE_JPEG = "image/jpeg";
        static constexpr const char * MIME_TYPE_IMAGE_PJPEG = "image/pjpeg";
        static constexpr const char * MIME_TYPE_IMAGE_PNG = "image/png";
        static constexpr const char * MIME_TYPE_IMAGE_SVG = "image/svg+xml";
        static constexpr const char * MIME_TYPE_IMAGE_TIFF = "image/tiff";
        static constexpr const char * MIME_TYPE_IMAGE_ICO = "image/vnd.microsoft.icon";
        static constexpr const char * MIME_TYPE_IMAGE_WBMP = "image/vnd.wap.wbmp";
        static constexpr const char * MIME_TYPE_IMAGE_WEBP = "image/webp";


        /*
         * Message
         */
        static constexpr const char * MIME_TYPE_MESSAGE_HTTP = "message/http";
        static constexpr const char * MIME_TYPE_MESSAGE_IMDN = "message/imdn+xml";
        static constexpr const char * MIME_TYPE_MESSAGE_EMAIL = "message/partial";
        static constexpr const char * MIME_TYPE_MESSAGE_EMAIL_2 = "message/rfc822";

        /*
         * Model
         */
        static constexpr const char * MIME_TYPE_MODEL_EXAMPLE = "model/example";
        static constexpr const char * MIME_TYPE_MODEL_IGS = "model/iges";
        static constexpr const char * MIME_TYPE_MODEL_MSH = "model/mesh";
        static constexpr const char * MIME_TYPE_MODEL_WRL = "model/vrml";
        static constexpr const char * MIME_TYPE_X3D_ISO = "model/x3d+binary";
        static constexpr const char * MIME_TYPE_X3D_ISO_2 = "model/x3d+vrml";
        static constexpr const char * MIME_TYPE_X3D_ISO_3 = "model/x3d+xml";

        /*
         * Multipart
         */

        static constexpr const char * MIME_TYPE_MULTIPART_MIXED = "multipart/mixed";
        static constexpr const char * MIME_TYPE_MULTIPART_ALTERNATIVE = "multipart/alternative";
        static constexpr const char * MIME_TYPE_MULTIPART_RELATED = "multipart/related";
        static constexpr const char * MIME_TYPE_MULTIPART_WEB_FORM = "multipart/form-data";
        static constexpr const char * MIME_TYPE_MULTIPART_SIGNED = "multipart/signed";
        static constexpr const char * MIME_TYPE_MULTIPART_ENCRYPTED = "multipart/encrypted";
        static constexpr const char * MIME_TYPE_MULTIPART_BYTES_RANGES = "multipart/byteranges; boundary=3d6b6a416f9b5";

        /*
         * Text
         */
        static constexpr const char * MIME_TYPE_TEXT_CMD = "text/cmd";
        static constexpr const char * MIME_TYPE_TEXT_CSS = "text/css";
        static constexpr const char * MIME_TYPE_TEXT_CSV = "text/csv";
        static constexpr const char * MIME_TYPE_TEXT_HTML = "text/html; charset=utf-8";
        static constexpr const char * MIME_TYPE_TEXT_JAVASCRIPT = "text/javascript";
        static constexpr const char * MIME_TYPE_TEXT_PLAIN = "text/plain";
        static constexpr const char * MIME_TYPE_TEXT_PHP = "text/php";
        static constexpr const char * MIME_TYPE_TEXT_XML = "text/xml";
        static constexpr const char * MIME_TYPE_TEXT_MARKDOWN = "text/markdown";
        static constexpr const char * MIME_TYPE_TEXT_CACHE_MANIFEST = "text/cache-manifest";

        /*
         * Video
         */
        static constexpr const char * MIME_TYPE_VIDEO_MPEG = "video/mpeg";
        static constexpr const char * MIME_TYPE_VIDEO_MP4 = "video/mp4";
        static constexpr const char * MIME_TYPE_VIDEO_OGG = "video/ogg";
        static constexpr const char * MIME_TYPE_VIDEO_QUICKTIME = "video/quicktime";
        static constexpr const char * MIME_TYPE_VIDEO_WEBM = "video/webm";
        static constexpr const char * MIME_TYPE_VIDEO_WINDOWS_MEDIA_VIDEO = "video/x-ms-wmv";
        static constexpr const char * MIME_TYPE_VIDEO_FLV = "video/x-flv";
        static constexpr const char * MIME_TYPE_VIDEO_GPP = "video/3gpp";
        static constexpr const char * MIME_TYPE_VIDEO_GPP2 = "video/3gpp2";

        /*
         * VND
         */
        static constexpr const char * MIME_TYPE_VND_OPEN_DOCUMENT_TEXT = "application/vnd.oasis.opendocument.text";
        static constexpr const char * MIME_TYPE_VND_OPEN_DOCUMENT_SPREADSHEET = "application/vnd.oasis.opendocument.spreadsheet";
        static constexpr const char * MIME_TYPE_VND_OPEN_DOCUMENT_PRESENTATION = "application/vnd.oasis.opendocument.presentation";
        static constexpr const char * MIME_TYPE_VND_OPEN_DOCUMENT_GRAPHICS = "application/vnd.oasis.opendocument.graphics";
        static constexpr const char * MIME_TYPE_VND_MICROSOFT_EXCEL = "application/vnd.ms-excel";
        static constexpr const char * MIME_TYPE_VND_MICROSOFT_EXCEL_2007 = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        static constexpr const char * MIME_TYPE_VND_MICROSOFT_POWERPOINT = "application/vnd.ms-powerpoint";
        static constexpr const char * MIME_TYPE_VND_MICROSOFT_POWERPOINT_2007 = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        static constexpr const char * MIME_TYPE_VND_MICROSOFT_WORD = "application/msword";
        static constexpr const char * MIME_TYPE_VND_MICROSOFT_WORD_2007 = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        static constexpr const char * MIME_TYPE_VND_MOZILLA_XUL = "application/vnd.mozilla.xul+xml";
        static constexpr const char * MIME_TYPE_VND_KML = "application/vnd.google-earth.kml+xml";

        /*
         * X
         */
        static constexpr const char * MIME_TYPE_X_FORM_ENCODED_DATA = "application/x-www-form-urlencoded";
        static constexpr const char * MIME_TYPE_X_DVI = "application/x-dvi";
        static constexpr const char * MIME_TYPE_X_LATEX = "application/x-latex";
        static constexpr const char * MIME_TYPE_X_TRUE_TYPE = "application/x-font-ttf";
        static constexpr const char * MIME_TYPE_X_ADOBE_FLASH = "application/x-shockwave-flash";
        static constexpr const char * MIME_TYPE_X_STUFFIT = "application/x-stuffit";
        static constexpr const char * MIME_TYPE_X_RAR = "application/x-rar-compressed";
        static constexpr const char * MIME_TYPE_X_TARBALL = "application/x-tar";
        static constexpr const char * MIME_TYPE_X_JQUERY = "text/x-jquery-tmpl";

        /*
         * X-PKCS
         */
        static constexpr const char * MIME_TYPE_X_PKCS_P12 = "application/x-pkcs12";
        static constexpr const char * MIME_TYPE_X_PKCS_PFX = "application/x-pkcs12";
        static constexpr const char * MIME_TYPE_X_PKCS_P7B = "application/x-pkcs7-certificates";
        static constexpr const char * MIME_TYPE_X_PKCS_SPC = "application/x-pkcs7-certificates";
        static constexpr const char * MIME_TYPE_X_PKCS_P7R = "application/x-pkcs7-certreqresp";
        static constexpr const char * MIME_TYPE_X_PKCS_P7C = "application/x-pkcs7-mime";
        static constexpr const char * MIME_TYPE_X_PKCS_P7M = "application/x-pkcs7-mime";
        static constexpr const char * MIME_TYPE_X_PKCS_P7S = "application/x-pkcs7-signature";
        
        static std::unordered_map<std::string, std::string> generateMimeTypesMap();
    };
}