/**
 * @file   httpcmd.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Thu May 12 17:23:09 2011
 *
 * @brief
 *
 *
 */
#include "httpcmd.h"
#include "blah.hpp"
#include "minIni.h"
#include  <cstdio>
#include  <malloc.h>
#include  <cstring>

#define MAX_FILE_LENGTH 20000


WriterMemoryClass::WriterMemoryClass()
{
    this->m_pBuffer = NULL;
    this->m_pBuffer = (char*) malloc(MAX_FILE_LENGTH * sizeof(char));
    this->m_Size = 0;
}

WriterMemoryClass::~WriterMemoryClass()
{
    if (this->m_pBuffer)
        free(this->m_pBuffer);
}

void* WriterMemoryClass::Realloc(void* ptr, size_t size)
{
    if(ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
}

size_t WriterMemoryClass::WriteMemoryCallback(char* ptr, size_t size, size_t nmemb)
{
    // Calculate the real size of the incoming buffer
    size_t realsize = size * nmemb;

    // (Re)Allocate memory for the buffer
    m_pBuffer = (char*) Realloc(m_pBuffer, m_Size + realsize);

    // Test if Buffer is initialized correctly & copy memory
    if (m_pBuffer == NULL) {
        realsize = 0;
    }

    memcpy(&(m_pBuffer[m_Size]), ptr, realsize);
    m_Size += realsize;

    return realsize;
};


void WriterMemoryClass::print()
{
    std::cout << "Size: " << m_Size << std::endl;
    std::cout << "Content: " << std::endl << m_pBuffer << std::endl;
}

size_t WriterMemoryClass:: write(std::string & s)
{
    s.clear();
    if ( m_pBuffer)
    {
        s = std::string (m_pBuffer, m_Size);
        return m_Size;
    }
    else
    {
        throw cURLpp::RuntimeError("Null Buffer");
    }
}

HttpCmd::HttpCmd()
{
    version = "03";

    ini = new minIni("properties.ini");

    prefix = ini->gets("Server","prefix");

    postfix = ini->gets("Server","postfix");

    url = ini->gets("Server", "serverurl");

    secretkey = ini->gets("Server","secretkey");



    // Set the writer callback to enable cURL
    // to write result in a memory area
    curlpp::types::WriteFunctionFunctor functor(&mWriterChunk,
                                                &WriterMemoryClass::WriteMemoryCallback);
    curlpp::options::WriteFunction *test = new curlpp::options::WriteFunction(functor);
    httprequest.setOpt(test);


    using namespace curlpp::Options;
    httprequest.setOpt(new Verbose(true));
    httprequest.setOpt(new Url(url));
    httprequest.setOpt(new curlpp::options::FileTime(true));
}


size_t HttpCmd::readData(const std::string & s)
{
    try{
        using namespace curlpp::Options;
        std::istringstream istream(s);
        char buf[50];
        std::list<std::string> headers;
        headers.push_back("Content-Type: */*");
        sprintf(buf, "Content-Length: %d", s.size());
        headers.push_back(buf);
        httprequest.setOpt(new ReadStream(&istream));
        httprequest.setOpt(new InfileSize(s.size()));
        httprequest.setOpt(new Upload(true));
        httprequest.setOpt(new HttpHeader(headers));
    }
    catch ( curlpp::LogicError & e )
    {
        std::cout << e.what() << std::endl;
    }
    catch ( curlpp::RuntimeError & e )
    {
        std::cout << e.what() << std::endl;
    }
    return s.size();
}

size_t HttpCmd::writeData(std::string & s )
{
    try {
        s.clear();
        return  mWriterChunk.write(s);
    }
    catch ( curlpp::LogicError & e )
    {
        std::cout << e.what() << std::endl;
    }
    catch ( curlpp::RuntimeError & e )
    {
        std::cout << e.what() << std::endl;
    }
}

std::string HttpCmd::encrypt( const std::string & s )
{
    std::string data(s);

    for(size_t i = 0; i < data.size(); i++)
        data[i] ^= secretkey[i % secretkey.size()];
    return data;
}


std::string HttpCmd::decrypt( const std::string & s)
{
    return encrypt(s);
}


HttpCmd::~HttpCmd()
{
    if ( ini )
        delete ini;
}



void HttpConnect::run(void *)
{
    std::ostringstream os;
    os << version <<"1"<<host<<":"<<port;
    this->readData(os.str());
}
