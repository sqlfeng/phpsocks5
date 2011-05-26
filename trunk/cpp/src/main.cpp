
#include<iostream>
#include "minIni.h"
#include "ServerSocket.h"
#include "TThreadPool.hh"
#include "blah.hpp"
#include "SocketException.h"
#include "packet.h"
#include "HttpTunnel.h"

class A{

};

class B:public A, public TThreadPool::TJob{
public:
    void run(void * data)
    {
        int i=*(int *)data;
        std::cout<<"test "<<i<<std::endl;
        sleep(2);
        std::cout<<"test done "<<i <<std::endl;
    }
};

int main()
{
    tp_init(10);
    // while(1)
    //{
        HttpTunnel tunnel;
        tp_run(&tunnel,NULL,false);
        sleep(2);
        //}
    tp_sync_all();
    tp_done();
}
