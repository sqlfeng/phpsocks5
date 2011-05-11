
#include<iostream>
#include "minIni.h"
#include "ServerSocket.h"
#include "TThreadPool.hh"
#include "blah.hpp"
#include "SocketException.h"
#include "packet.h"
class SockServer:public TThreadPool::TJob
{

  ServerSocket *server;

public:
  
  SockServer(ServerSocket *ser)
  {
    server =  ser;
  }

  ~SockServer()
  {
    delete server;
  }
  
  virtual void run( void *)
  {
    ServerSocket new_sock;    
    server->accept ( new_sock);
    
    try{
      std::string data;
      new_sock >> data;
      cc_hs_request req;
      req.parse(data);
      LOG<<"Pasr hand shack request";
      cc_hs_response rsp;
      std::string send;
      rsp.dump(send);
      new_sock << send;
      new_sock >> data;
      cc_request cc_req;
      cc_req.parse(data);
      std::string host = cc_req.getHost();
      std::cout<<"host = "<<host<<std::endl;
      cc_req.getPort();
      cc_response cc_rsp((cc_response::REP)0);
      cc_rsp.dump(send);
      new_sock << send;
      new_sock >> data;
      std::cout<<data<<std::endl;
    }
    catch(SocketException & ){}

  }
};
int main()
{

  tp_init(25);

  ServerSocket server(10080);
  while(1)
    {
      SockServer * ser = new SockServer( & server);
      tp_run(ser,NULL,false);
    }
  tp_sync_all();
  tp_done();

}
