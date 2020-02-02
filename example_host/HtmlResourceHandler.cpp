#include "HtmlResourceHandler.h"
#include <iostream>

struct HtmlResource
{

};

HtmlResource* HtmlResourceHandler::OpenResource( const char* pHost, const char* pPath )
{
    return nullptr;
}

void HtmlResourceHandler::CloseResource( HtmlResource* resource )
{
    delete resource;
}

size_t HtmlResourceHandler::GetLength( HtmlResource* resource )
{
    return 0;
}

void HtmlResourceHandler::ReadData( HtmlResource* resource, char* pDestination, size_t length )
{

}

void HtmlResourceHandler::Message( const char* data )
{
    std::cout << data << std::endl;
}