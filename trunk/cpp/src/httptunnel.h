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


class ServerSocket;


class HttpTunnel:public TThreadPool::TJob
{

    // socket between firefox and cppclient.
    ServerSocket *socks;

};

