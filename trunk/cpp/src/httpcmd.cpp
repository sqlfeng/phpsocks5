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

#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>

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

    size_t realsize = size * nmemb;

    m_pBuffer = (char*) Realloc(m_pBuffer, m_Size + realsize);

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

    indata.clear();
    outdata.clear();

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
        LOG<<"Start to write data " << s<<std::endl;
        httprequest.setOpt(new curlpp::options::FileTime(true));
        httprequest.setOpt(new curlpp::options::Verbose(true));

        std::string body = encrypt(s);

        std::list<std::string> header;
        header.push_back("Content-Type: */*");

        httprequest.setOpt(new curlpp::options::HttpHeader(header));

        httprequest.setOpt(new curlpp::options::PostFields(body));
        httprequest.setOpt(new curlpp::options::PostFieldSize(body.size()));

        httprequest.setOpt(curlpp::options::CookieList(""));
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
        mWriterChunk.write(s);
        s = decrypt(s);
        return s.size();
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

void HttpCmd::putCookies(const std::list<std::string> & cookieslist)
{
    if ( cookieslist.empty() )
    {
        LOG<< "Cookie list empty!";
        return;
    }
    else
    {
        try{
            for( std::list<std::string>::const_iterator it = cookieslist.begin(); it!= cookies.end(); it ++)
            {
                httprequest.setOpt(curlpp::options::CookieList(*it));
            }
        }catch ( curlpp::LogicError & e )
        {
            LOG<<"Logic Error";
            std::cout << e.what() << std::endl;
        }
        catch ( curlpp::RuntimeError & e )
        {
            LOG<<"RuntimeError";
            std::cout << e.what() << std::endl;
        }
    }
}

std::list<std::string> HttpCmd::getCookies()
{

    if (cookies.empty())
    {
        LOG<< "Cookie list is empty!";
    }
    return cookies;
}

HttpCmd::~HttpCmd()
{
    if ( ini )
        delete ini;
}

HttpConnect::HttpConnect(const std::string & host, const int port ): HttpCmd()
{
    this->host = host;
    this->port = port;
}

void HttpConnect::setHostPort( const std::string & host, const int & port)
{
    this->host = host;
    this->port = port;
}

void HttpConnect::run(void *)
{
    if ( host.empty())
    {
        LOG<<"HOST Empty";
        throw curlpp::LogicError("Host Empty");
    }

    try{

        std::ostringstream os;

        os << version <<"1"<<host<<":"<<port;

        std::cout<<os.str()<<std::endl;
        this->readData(os.str());

        httprequest.perform();

        std::string effURL;
        curlpp::infos::EffectiveUrl::get(httprequest, effURL);
        LOG << "Effective URL: " << effURL << std::endl;

        LOG << "Response code: "
            << curlpp::infos::ResponseCode::get(httprequest)
            << std::endl;

        curlpp::infos::CookieList::get(httprequest, cookies);

        if ( cookies.empty())
        {
            LOG<<"Cookie empty";
        }

    }catch ( curlpp::LogicError & e )
    {
        LOG<<"Logic Error";
        std::cout << e.what() << std::endl;
    }
    catch ( curlpp::RuntimeError & e )
    {
        LOG<<"RuntimeError";
        std::cout << e.what() << std::endl;
    }
}

HttpIdle::HttpIdle(std::list<std::string> cookieslist)
{

    this->putCookies(cookieslist);

}

void HttpIdle::run(void *)
{
    std::ostringstream os;

    os << version <<"2";

    this->readData(os.str());

    httprequest.perform();

    std::string output;

    while( writeData(output) >= 0 );
}

HttpSend::HttpSend(std::list<std::string> cookieslist)
{
    this->putCookies(cookieslist);
}

void HttpSend::run(void *)
{

    try{
        std::ostringstream os ;

        os<<version<<"3";

        this->readData(os.str());

        this->readData(indata);

        httprequest.perform();

        this->writeData(outdata);

    }catch ( curlpp::LogicError & e )
    {
        LOG<<"Logic Error";
        std::cout << e.what() << std::endl;
    }
    catch ( curlpp::RuntimeError & e )
    {
        LOG<<"RuntimeError";
        std::cout << e.what() << std::endl;
    }
}
