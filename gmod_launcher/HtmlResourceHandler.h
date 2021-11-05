#pragma once

#include "html/IHtmlResourceHandler.h"

class HtmlResourceHandler : public IHtmlResourceHandler
{
    HtmlResource* OpenResource( const char* pHost, const char* pPath ) override;
    void CloseResource( HtmlResource* resource ) override;

    size_t GetLength( HtmlResource* resource ) override;
    void ReadData( HtmlResource* resource, char* pDestination, size_t length ) override;

    void Message( const char* data ) override;
};

