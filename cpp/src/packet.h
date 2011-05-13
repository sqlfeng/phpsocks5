/**
 * @file   packet.h
 * @author Devil Wang <wxjeacen@gmail.com>
 * @date   Thu May 12 10:32:17 2011
 *
 * @brief packet for cpp client and firefox communication
 *
 *
 */

#ifndef _PACKET_H_
#define _PACKET_H_

#include <string>
struct packet{

    packet();
    packet( const std::string &);
    ~packet();
    //  virtual bool parse(const std::string &) ;
    //  virtual bool dump (std::string &);

    std::string entry;

protected:
    bool validate;

};

struct request:public packet{

    virtual bool parse( const std::string &){ return false; }
};

struct response:public packet{

    //  virtual bool dump ( std::string &){}

};

class cc_hs_request: public request{

public:

    cc_hs_request();
    cc_hs_request(const std::string &s);
    ~cc_hs_request();

    virtual bool parse(const std::string &);
    //virtual bool dump( std::string &);

private:
    char ver;
    char nmethods;
    char methods;
};

class cc_hs_response: public response{
public:
    cc_hs_response();
    ~cc_hs_response();
    //virtual bool parse(const std::string &);
    virtual bool dump( std::string &);
private:

    char ver;
    char method;

};

class cc_request:public request{

public:
    enum CMD{
        CONNECT=0x01,
        BIND=0x02,
        UDP_ASSOCIATE=0x03
    };

    enum ATYP{
        IPV4 = 0x01,
        HOST = 0x03,
        IPV6 = 0x04
    };

    cc_request();
    ~cc_request();
    std::string getHost();
    int getPort();

    CMD getCMD();
    virtual bool parse(const std::string &);
    //  virtual bool dump ( std::string &);

private:

    char ver;
    CMD cmd;
    char rsv;
    ATYP atyp;
    char * dist_addr;
    char port[2];
};


class cc_response: public response{

public:

    enum REP{
        SUCCESS=0x00,
        SOCKKET_FAIL,
        CONNECTION_FAIL,
        NETWORK_UNREACH,
        HOST_UNREACH,
        CONNECTION_REFUSE,
        TTL_EXPIRE,
        CMD_UNSUPPORT,
        ADDR_UNSUPPORT,
        UNASSIGNED
    };

    enum ATYP{
        IPV4 = 0x01,
        HOST= 0x03,
        IPV6= 0x04
    };
    cc_response();
    cc_response(REP);
    ~cc_response();
    //virtual bool parse(const std::string &);
    virtual bool dump( std::string &);

    void setResponse(REP);
    void setAddr(ATYP atyp, const char * host,const char * port);

private:
    char ver;
    REP rep;
    char rsv;
    ATYP atyp;
    char * bind_addr;
    char port[2];
};

#endif
