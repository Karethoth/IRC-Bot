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



  ExtensionManager *em = new ExtensionManager();
  em->Update();


  IRC::Server *server = new IRC::Server( host, port );
  if( !server )
  {
    fprintf( stderr, "Setting up the server failed!" );
    return -1;
  }

  if( !server->Connect() )
  {
    printf( "connecting failed" );
  }

  while( true )
  {

    if( !server )
    {
      printf( "missing server!" );
      return -1;
    }

    if( !server->IsConnected() )
    {
      printf( "Server disconnected, connecting again\n" );
      server->Connect();
      continue;
    }

    em->Update();

    em->HandleCommands( server );
  }

  if( server )
  {
    server->Disconnect();
    delete server;
  }

  return 0;
}

