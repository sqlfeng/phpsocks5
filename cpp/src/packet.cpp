
#include <exception>
#include "packet.h"
#include "blah.hpp"
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

packet::packet()
{
  entry.clear();
  validate = false;
}

packet::~packet()
{
  entry.clear();
}
packet::packet(const std::string & s)
{
  entry.clear();
  entry = s;
  validate = false;
}


cc_hs_request::cc_hs_request(){
  entry.clear();
  validate = false;
}

cc_hs_request::~cc_hs_request(){
  entry.clear();
}

cc_hs_request::cc_hs_request(const std::string & s)
{
  try{
      parse( s );
  }
  catch( std::exception e)
    {
      LOG<<"construction error "<<std::endl;
      validate = false;
    }
}

bool cc_hs_request::parse(const std::string & s )
{
  entry.clear();
  if ( s.empty())
    {
      LOG << "parse error" <<std::endl;
      throw std::exception();
    }
  
  entry = s;
  ver = entry[0];
  nmethods = entry[1];
  methods = entry[2];
  
  if ( (int) ver != 5 )
    {
      validate = false;
      return false;
    }
  
  validate = true;
  return true;

}

// bool cc_hs_request::dump( std::string & s)
// {
//   if ( entry.empty() )
//     {
//       LOG << " dump error "<<std::endl;
//       throw std::exception();
//     }
  
//   if ( validate == false)
//     {
//       s.clear();
//       return false;
//     }

//   char buf[3] = {ver, nmethods, methods};
//   s = std::string(buf,3);
//   return true;
// }

cc_hs_response::cc_hs_response()
{
  entry.clear();
  ver = 5;
  method = 0;
  char buf[2]= {ver,method};
  entry = std::string(buf,2);
  validate = true;
}

cc_hs_response::~cc_hs_response()
{
  entry.clear();
}

// bool cc_hs_response::parse( const std::string &s )
// {
//   if ( s.empty()|| s.size() != 2 )
//     {
//       validate = false;
//       throw std::exception();
//     }
//   entry = s ;
//   ver = entry[0];
//   if ( (int) ver != 5 )
//     return false;
//   method = entry[1];
//   return true;
// }

bool cc_hs_response::dump( std::string & s)
{
  s.clear();
  if ( ! validate )
    return false;
  s = entry;
  return true;
}


cc_request::cc_request()
{
  entry.clear();
  validate = false;
}

cc_request::~cc_request()
{
  if ( ! dist_addr)
    delete dist_addr;
  entry.clear();
}

std::string cc_request::getHost()
{
  struct sockaddr_in sa;
  memcpy(& (sa.sin_addr), dist_addr, INET_ADDRSTRLEN);
  char buf[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &(sa.sin_addr), buf, INET_ADDRSTRLEN);
  buf[INET_ADDRSTRLEN] = '\0';
  return std::string(buf);
}

int cc_request::getPort()
{
  if (! validate || entry.empty() )
    throw std::exception();
  int Port = (int)port[0]<<8;
  Port =Port | (int) port[1];
  return Port;
}

bool cc_request::parse(const std::string & s)
{
  entry.clear();
  if ( s.empty())
    throw std::exception();
  
  entry = s;
  
  ver = entry[0];
  
  cmd = static_cast<CMD> (entry[1]);

  if ( (int) entry[2] != 0x00 )
    throw std::exception();
  rsv = 0x00;
  
  atyp = static_cast<ATYP>(entry[3]);
  
  switch (atyp)
    {
    case IPV4 :
      dist_addr = new char [4];
      memcpy( dist_addr, & entry[4], 4);
      dist_addr[4] = '\0';
      memcpy(port, & entry[8],2);
      port[2]= '\0';
      break;
    case HOST:
      dist_addr = NULL;
      break;
    case IPV6:
      dist_addr = new char [16];
      memcpy(dist_addr, & entry[4],16);
      memcpy(port, & entry[20],2);
      break;
    default:
      LOG<< "ATYP Error "<<std::endl;
      break;
    }

  validate = true;
  return true;
}

cc_response::cc_response()
{
  entry.clear();
  char buf[10]={5,0,0,1,0,0,0,0,0,0};
  entry= std::string(buf,10);  
  validate = true;
}

cc_response::cc_response(REP rep)
{
  entry.clear();
  char buf[10]={5,0,0,1,0,0,0,0,0,0};
  buf[1] = static_cast<char>( rep );
  entry= std::string(buf,10);  
  validate = true;
}

cc_response::~cc_response()
{
  entry.clear();
}

void cc_response::setResonse(cc_response::REP rep)
{
  if (!validate || entry.empty())
    throw std::exception();
  
  entry[1] = static_cast<char>(rep);
}

bool cc_response::dump(std::string & s)
{
  s.clear();
  if ( !validate || entry.empty())
    throw std::exception();
  
  s = entry;
  return true;
}


