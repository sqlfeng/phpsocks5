/**
 * @file   httptunnel.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Thu May 12 16:26:17 2011
 *
 * @brief
 *
 *
 */

#include "minIni.h"
#include "TThreadPool.hh"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include "ServerSocket.h"

class minIni;

class HttpTunnel:public TThreadPool::TJob
{
public:
    // socket between firefox and cppclient.

    class Php_Pasive_Retrive:public TThreadPool::TJob
    {
        void run(void *);
    };

    class Php_Actice_Rretrive: public TThreadPool::TJob
    {
        void run(void *);
    };


    class Php_Idle:public TThreadPool::TJob
    {
        void run(void *);
    };

    HttpTunnel();

    ~HttpTunnel();

    void run( void * );

protected:

    ServerSocket socks;

    std::string host;

    int port;

    minIni * ini;

    void handshake();

    void phpconnect();

};

