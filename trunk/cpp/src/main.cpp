
#include<iostream>
#include "minIni.h"
#include "ServerSocket.h"
#include "TThreadPool.hh"
#include "blah.hpp"
#include "SocketException.h"

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
      for( int i=0 ; i < data.size(); i ++)
        {
          LOG <<(int)data[i];
        }
      std::cout<<std::endl;
      LOG<<"send response"<<std::endl;
      char buf[2] = {0x05, 0x00};
      std::string send(buf);
      new_sock << send;
      for( int i=0 ; i < data.size(); i ++)
        {
          std::cout <<(int)send[i];
        }
      std::cout<<std::endl;
      LOG<<"send done" <<std::endl;
      sleep(1);
      new_sock >> data;
     
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
