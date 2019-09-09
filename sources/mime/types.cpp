#include "types.h"

std::unordered_map<std::string, std::string> onyxup::MimeType::generateMimeTypesMap(){
    std::unordered_map<std::string, std::string> types;

    types["css"] = MIME_TYPE_TEXT_CSS;
    types["html"] = MIME_TYPE_TEXT_HTML;
    types["jpg"] = MIME_TYPE_IMAGE_JPEG;
    types["png"] = MIME_TYPE_IMAGE_PNG;
    types["json"] = MIME_TYPE_APPLICATION_JSON;
    types["jpeg"] = MIME_TYPE_IMAGE_JPEG;
    types["js"] = MIME_TYPE_APPLICATION_JAVASCRIPT;
    types["woff2"] = MIME_TYPE_APPLICATION_FONT_WOFF;
    types["svg"] = MIME_TYPE_IMAGE_SVG;
    
    return types;
}