#include "MimeType.hpp"

MimeType::MimeType(void)
{
}

MimeType::~MimeType(void)
{
}

MimeType::MimeType(const MimeType &object)
{
    *this = object;
}

MimeType &MimeType::operator=(const MimeType &object)
{
    if (this == &object)
        return (*this);
    return (*this);
}

void MimeType::setMimeTypeMap(const std::string &typeBlock)
{
}