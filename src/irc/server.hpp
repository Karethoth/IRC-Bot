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


   protected:
    std::map<std::string, CommandHandler>  callbackMap;
    std::vector<Command>                  *GetCommands();


   public:
    Server( std::string host, int port );
    ~Server();

    bool Connect();
    void Disconnect();

    bool Write( std::string msg );

    bool HandleCommands();
    bool SetCommandHandler( std::string, CommandHandler );
    
    bool IsConnected(){ return (state != NOT_CONNECTED); }

    ServerState GetState(){ return state; }
    ServerState SetState( ServerState s){ state = s; }
  };
}


#endif

