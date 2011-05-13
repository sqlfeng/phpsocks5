/**
 * @file   httptunnel.cpp
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Thu May 12 16:26:36 2011
 *
 * @brief
 *
 *
 */

#include "HttpTunnel.h"
#include "minIni.h"
#include "blah.hpp"
#include "packet.h"
#include <exception>
#include "httpcmd.h"
HttpTunnel::HttpTunnel()
{
    ini = new minIni("properties.ini");

    int localport = ini->geti("Server","localport");

    LOG<<"Listening on port "<< localport;

    ServerSocket oldsock(localport);

    oldsock.accept(socks);

    try{
        handshake();
    }catch(...)
    {
        LOG<<"HttpTunnel Construction Fail";
    }
}

HttpTunnel::~HttpTunnel()
{
}


void HttpTunnel::run(void *)
{
    // try{
    phpconnect();
    //}
    //catch(...)
    //{
    //    LOG<<"Connection error";
    //}
}

void HttpTunnel::handshake()
{
    cc_hs_request hs_request;
    std::string data;
    socks>> data;
    hs_request.parse(data);

    cc_hs_response hs_response;

    if ( ! hs_response.dump(data))
    {
        throw std::exception();
    }

    socks << data;

    cc_request request;
    socks >> data;

    if ( ! request.parse(data))
    {
        LOG<<"request parse error";
        throw std::exception();
    }

    host = request.getHost();
    port = request.getPort();
    cc_response response;

    if ( request.getCMD() != static_cast<cc_request::CMD> (0))
    {

        response.setResponse(static_cast<cc_response::REP>(0));
    }
    else
    {
        response.setResponse(static_cast<cc_response::REP>(7));
        LOG<<"Command unsuported";
    }
    response.dump(data);

    socks<< data;

    LOG<<"Hand shaked";

}

void HttpTunnel::phpconnect()
{
    if ( host.empty())
    {
        LOG<<"Host Empty";
        throw std::exception();
    }

    HttpCmd * cmd = new HttpConnect(host, port);

    tp_run(static_cast<HttpConnect*>(cmd));

    tp_sync(static_cast<HttpConnect*>(cmd));

    LOG<<"Connection has been registered";
}
