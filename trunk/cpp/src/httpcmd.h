/**
 * @file   httpcmd.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Thu May 12 17:15:44 2011
 *
 * @brief  Http Cmd class set
 *
 *
 */
#include "TThreadPool.hh"
#include <string>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Exception.hpp>
#include <sstream>

class minIni;

class WriterMemoryClass
{
public:

    WriterMemoryClass();

    ~WriterMemoryClass();

    void * Realloc(void * ptr, size_t );

    size_t WriteMemoryCallback(char *  ptr, size_t size, size_t nmemb);

    void print ();

    size_t write(std::string &);

    char *m_pBuffer;

    size_t m_Size;
};


class HttpCmd{

protected:
    minIni * ini;

    std::string version;

    std::string prefix;

    std::string postfix;

    std::string secretkey;

    std::string url;

public:
    HttpCmd();
    ~HttpCmd();
    cURLpp::Cleanup cleanup;
    cURLpp::Easy httprequest;

    WriterMemoryClass mWriterChunk;

    size_t readData(const std::string & s );

    size_t writeData(std::string &) ;

    std::string encrypt(const std::string &);

    std::string decrypt(const std::string &);

};

class HttpConnect:public HttpCmd,public TThreadPool::TJob
{
    std::string host;

    int port;

public:
    HttpConnect():HttpCmd(){}

    ~HttpConnect(){}

    void run(void *);

};
