#include <dlfcn.h>
#include <sys/stat.h>
#include <iostream>
#include "irc/irc.hpp"
#include "extension_manager.hpp"


using std::string;


const char *libircPath = "/opt/lib/libirc.so";


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


  ExtensionManager *em = new ExtensionManager();
  em->Update();


  while( true )
  {
    // Load libirc if it has been updated.
    libircTimestamp = GetTimestamp( libircPath );
    if( libircChanged < libircTimestamp )
    {
      if( libircHandle )
      {
        dlclose( libircHandle );
      }

      libircHandle = dlopen( libircPath, RTLD_LAZY );
      if( !libircHandle )
      {
        fprintf( stderr, "Couldn't load libirc.so %s\n", dlerror() );
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

    em->HandleCommands( server );
  }

  if( server )
  {
    server->Disconnect();
    IrcServerDestroyer( server );
  }

  return 0;
}

