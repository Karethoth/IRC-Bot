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

#include <sqlite3.h>
#include "../database.hpp"

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

    sqlite3 *db;
    std::map<std::string,std::string> settings;


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

    virtual bool Nick( std::string newNick );

    virtual bool IsConnected(){ return (state != NOT_CONNECTED); }

    virtual ServerState GetState(){ return state; }
    virtual void SetState( ServerState s){ state = s; }

    virtual int GetSocket(){ return sockfd; }
    virtual void SetSocket( int sock ){ sockfd = sock; }

    virtual bool GetCommands( std::vector<IRC::Command> *commands );

    virtual void SetDB( sqlite3 *database ){ db = database; }
    virtual sqlite3 *GetDB(){ return db; }

    virtual bool ReloadSettings();
    virtual std::map<std::string,std::string> GetServerSettings()
    { return settings; }

    // For extensions, that interact with other extension(s)
    void *extensionManager;
  };
}


#endif

