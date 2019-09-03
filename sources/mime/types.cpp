#include "types.h"

std::unordered_map<std::string, std::string> onyxup::MimeType::generateMimeTypeMap(){
    std::unordered_map<std::string, std::string> mime_types;
    
    mime_types["css"] = MIME_TYPE_TEXT_CSS;
    mime_types["html"] = MIME_TYPE_TEXT_HTML;
    mime_types["jpg"] = MIME_TYPE_IMAGE_JPEG;
    mime_types["png"] = MIME_TYPE_IMAGE_PNG;
    mime_types["json"] = MIME_TYPE_APPLICATION_JSON;
    mime_types["jpeg"] = MIME_TYPE_IMAGE_JPEG;
    mime_types["js"] = MIME_TYPE_APPLICATION_JAVASCRIPT;
    mime_types["woff2"] = MIME_TYPE_APPLICATION_FONT_WOFF;
    mime_types["svg"] = MIME_TYPE_IMAGE_SVG;
    
    return mime_types;
}