#include <dlfcn.h>
#include <sys/stat.h>
#include <iostream>
#include "irc/irc.hpp"


using std::string;




const char *libircPath = "/opt/lib/libirc.so";
const char *libbotPath = "/opt/lib/libbot.so";


int GetTimestamp( const char *path )
{
  struct stat st;

  int err = stat( path, &st );
  if( err != 0 )
  {
    fprintf( stderr, "Couldn't open '%s'\n", path );
    return -1;
  }

  return st.st_mtime;
}



int main()
{

  // libbot
  void  *libbotHandle    = NULL;
  int    libbotChanged   = 0;
  int    libbotTimestamp = 0;
  bool (*SetCommandHandlers)(IRC::Server*);

  // libirc
  void *libircHandle    = NULL;
  int   libircChanged   = 0;
  int   libircTimestamp = 0;

  // Server related
  string host   = "irc.quakenet.org";
  int    port   = 6667;
  int    sockfd = -1;
  IRC::ServerState serverState = IRC::NOT_CONNECTED;

  IRC::Server *(*IrcServerMaker)();
  void         (*IrcServerDestroyer)(IRC::Server*);
  IRC::Server *server = NULL;


  while( true )
  {
    // Load libbot if it has been updated.
    libbotTimestamp = GetTimestamp( libbotPath );
    if( libbotChanged < libbotTimestamp )
    {
      if( libbotHandle )
      {
        dlclose( libbotHandle );
      }

      libbotHandle = dlopen( libbotPath, RTLD_NOW );
      if( !libbotHandle )
      {
        fprintf( stderr, "Couldn't load libbot.so\n" );
        return -1;
      }
      printf( "libbot.so loaded.\n" );

      SetCommandHandlers = (bool(*)(IRC::Server*))dlsym( libbotHandle, "SetCommandHandlers" );
      if( !SetCommandHandlers )
      {
        fprintf( stderr, "Couldn't load libbot.so::SetCommandHandlers\n" );
        return -1;
      }
      printf( "libbot.so::SetCommandHandlers loaded.\n" );

      libbotChanged = libbotTimestamp;

      if( serverState != IRC::NOT_CONNECTED )
      {
        SetCommandHandlers( server );
      }
    }


    // Load libirc if it has been updated.
    libircTimestamp = GetTimestamp( libircPath );
    if( libircChanged < libircTimestamp )
    {
      if( libircHandle )
      {
        dlclose( libircHandle );
      }

      libircHandle = dlopen( libircPath, RTLD_NOW );
      if( !libircHandle )
      {
        fprintf( stderr, "Couldn't load libirc.so\n" );
        return -1;
      }
      printf( "libirc.so loaded.\n" );


      IrcServerMaker = (IRC::Server*(*)())dlsym( libircHandle, "ServerMaker" );
      if( !IrcServerMaker )
      {
        fprintf( stderr, "Couldn't load libirc.so::ServerMaker\n" );
        return -1;
      }
      printf( "libirc.so::ServerMaker loaded.\n" );


      IrcServerDestroyer = (void(*)(IRC::Server*))dlsym( libircHandle, "ServerDestroyer" );
      if( !IrcServerDestroyer )
      {
        fprintf( stderr, "Couldn't load libirc.so::ServerDestroyer\n" );
        return -1;
      }
      printf( "libirc.so::ServerDestroyer loaded.\n" );

      libircChanged = libircTimestamp;


      // Set up the server class.
      server = IrcServerMaker();
      if( !server )
      {
        fprintf( stderr, "Setting up the server failed!" );
        return -1;
      }

      server->Init( host, port, sockfd );
      server->SetState( serverState );

      // Set up the command handlers
      SetCommandHandlers( server );

      if( serverState == IRC::NOT_CONNECTED )
      {
        server->Connect();
        serverState = server->GetState();
        sockfd = server->GetSocket();
      }

      printf( "Server class's been got to work!" );
    }

    if( !server )
      continue;

    if( !server->IsConnected() )
    {
      printf( "Server disconnected, connecting again\n" );
      serverState = IRC::NOT_CONNECTED;
      server->Connect();
      continue;
    }

    server->HandleCommands();
  }

  if( server );
  {
    server->Disconnect();
    IrcServerDestroyer( server );
  }

  return 0;
}

