#include "TypeChecker.hpp"

TypeChecker::TypeChecker(void)
{
    _setMimeType();
}

TypeChecker::~TypeChecker(void)
{
}

TypeChecker::TypeChecker(const TypeChecker &object)
{
    *this = object;
}

TypeChecker &TypeChecker::operator=(const TypeChecker &object)
{
    if (this == &object)
        return (*this);
    _mimeTypeMap = object._mimeTypeMap;
    return (*this);
}

void TypeChecker::_setMimeType(void)
{
    _setTextType();
    _setImageType();
    _setApplicationType();
}

void TypeChecker::_setTextType(void)
{
    _mimeTypeMap["html"]  = "text/html";
    _mimeTypeMap["htm"]   = "text/html";
    _mimeTypeMap["shtml"] = "text/html";
    _mimeTypeMap["mml"]   = "text/mathml";
    _mimeTypeMap["txt"]   = "text/plain";
    _mimeTypeMap["jad"]   = "text/vnd.sun.j2me.app-descriptor ";
    _mimeTypeMap["wml"]   = "text/vnd.wap.wml";
    _mimeTypeMap["htc"]   = "text/x-component";
}

void TypeChecker::_setImageType(void)
{
    _mimeTypeMap["gif"]  = "image/gif";
    _mimeTypeMap["jpeg"] = "image/jpeg";
    _mimeTypeMap["jpg"]  = "image/jpeg";
    _mimeTypeMap["avif"] = "image/avif";
    _mimeTypeMap["png"]  = "image/png";
    _mimeTypeMap["svg"]  = "image/svg+xml";
    _mimeTypeMap["svgz"] = "image/svg+xml";
    _mimeTypeMap["tif"]  = "image/tiff";
    _mimeTypeMap["tiff"] = "image/tiff";
    _mimeTypeMap["wbmp"] = "image/vnd.wap.wbmp";
    _mimeTypeMap["webp"] = "image/webp";
    _mimeTypeMap["ico"]  = "image/x-icon";
    _mimeTypeMap["jng"]  = "image/x-jng";
    _mimeTypeMap["bmp"]  = "image/x-jng";
}

void TypeChecker::_setApplicationType(void)
{
    _mimeTypeMap["js"]   = "application/javascript";
    _mimeTypeMap["atom"] = "application/atom+xml";
    _mimeTypeMap["rss"]  = "application/rss+xml";
    _mimeTypeMap["jar"]  = "application/java-archive";
    _mimeTypeMap["war"]  = "application/java-archive";
    _mimeTypeMap["ear"]  = "application/java-archive";
    _mimeTypeMap["json"] = "application/json";
    _mimeTypeMap["hqx"]  = "application/mac-binhex40";
    _mimeTypeMap["doc"]  = "application/msword";
    _mimeTypeMap["pdf"]  = "application/pdf";
    _mimeTypeMap["ps"]   = "application/postscript";
    _mimeTypeMap["eps"]  = "application/postscript";
    _mimeTypeMap["ai"]   = "application/postscript";
    _mimeTypeMap["rtf"]  = "application/rtf";
    _mimeTypeMap["m3u8"] = "application/vnd.apple.mpegurl";
    _mimeTypeMap["kml"]  = "application/google-earth.kml+xml";
    _mimeTypeMap["kmz"]  = "application/google-earth.kmz";
    _mimeTypeMap["xls"]  = "application/ms-excel";
    _mimeTypeMap["oet"]  = "application/ms-fontobject";
    _mimeTypeMap["ppt"]  = "application/ms-powerpoint";
    _mimeTypeMap["odg"]  = "application/oasis.opendocument.graphics";
    _mimeTypeMap["odp"]  = "application/oasis.opendocument.presentation";
    _mimeTypeMap["ods"]  = "application/oasis.opendocument.spreadsheet";
    _mimeTypeMap["odt"]  = "application/oasis.opendocument.text";
    _mimeTypeMap["pptx"] =
        "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    _mimeTypeMap["xlsx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.sheet";
    _mimeTypeMap["docx"] =
        "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    _mimeTypeMap["wmlc"] = "application/vnd.wap.wmlc";
}