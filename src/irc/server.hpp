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

typedef void (*CommandHandler)(IRC::Command);


namespace IRC
{
  class Server
  {
   private:
    int         sockfd;
    std::string serverHost;
    int         serverPort;

    char       *buffer;
    ssize_t     bufferSize;

    bool connected;


   protected:
    std::map<std::string, CommandHandler> callbackMap;

    std::vector<Command> *GetCommands();


   public:
    Server( std::string host, int port );
    ~Server();

    bool Connect();
    void Disconnect();

    bool Write( std::string msg );

    bool HandleCommands();
    bool SetCallback( std::string, CommandHandler );
    
    bool IsConnected(){ return connected; }

  };
}


#endif

