#ifndef __IRC_SERVER__
#define __IRC_SERVER__

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <cstdio>
#include <cstring>
#include <string>

#include <map>
#include <vector>

#include "command.hpp"


typedef bool (*CommandHandler)(IRC::Command, void*);


namespace IRC
{
  enum ServerState
  {
    NOT_CONNECTED,
    SETTING_NICK,
    SETTING_USER,
    WORKING
  };


  class Server
  {
   private:
    int         sockfd;
    std::string serverHost;
    int         serverPort;

    char       *buffer;
    ssize_t     bufferSize;

    ServerState state;

    std::vector<std::string> channels;

   public:
    Server();
    Server( std::string host, int port );
    ~Server();

    virtual void Init( std::string host, int port, int sock );

    virtual bool Connect();
    virtual void Disconnect();

    virtual bool Write( std::string msg );

    virtual bool Join( std::string channel );
    virtual bool Part( std::string channel );

    virtual bool IsConnected(){ return (state != NOT_CONNECTED); }

    virtual ServerState GetState(){ return state; }
    virtual ServerState SetState( ServerState s){ state = s; }

    virtual int GetSocket(){ return sockfd; }
    virtual bool GetCommands( std::vector<IRC::Command> *commands );

    // For extensions, that interact with other extension(s)
    void *extensionManager;
  };
}


#endif

