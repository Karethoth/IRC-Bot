#include <dlfcn.h>
#include <sys/stat.h>
#include <iostream>
#include "irc/irc.hpp"


using std::string;


bool Authenticate( IRC::Command cmd, void *server )
{
  IRC::Server *srv = static_cast<IRC::Server*>( server );
  if( srv->GetState() == IRC::SETTING_NICK )
  {
    printf( "Setting nick\n" );
    srv->Write( "NICK KoukariBot\n" );
    srv->SetState( IRC::SETTING_USER );
  }
  else if( srv->GetState() == IRC::SETTING_USER )
  {
    printf( "Setting user\n" );
    srv->Write( "USER meh meh meh 8:KoukariBot\n" );
  }
  return true;
}



bool LoggedIn( IRC::Command cmd, void *server )
{
  IRC::Server *srv = static_cast<IRC::Server*>( server );
  if( std::string( cmd.command ).compare("001") )
  {
    srv->SetState( IRC::WORKING );
  }
}



bool Pong( IRC::Command cmd, void *server )
{
  IRC::Server *srv = static_cast<IRC::Server*>( server );
  std::string msg = "PONG :";
  msg.append( cmd.data );
  srv->Write( msg );
  srv->SetState( IRC::WORKING );
  return true;
}



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
  void *libircHandle    = NULL;
  int   libircChanged   = 0;
  int   libircTimestamp = 0;


  // Server related
  string host   = "irc.quakenet.org";
  int    port   = 6667;
  int    sockfd = -1;
  IRC::ServerState serverState = IRC::NOT_CONNECTED;

  IRC::Server* (*IrcServerMaker)();
  void         (*IrcServerDestroyer)(IRC::Server*);
  IRC::Server *server = NULL;

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

      if( serverState == IRC::NOT_CONNECTED )
      {
        server->Connect();
        serverState = server->GetState();
      }

      // Set up the command handlers.
      // -----------
      // This need to be imported to a library
      // to make it customizable at runtime.
      // As well as the commandHandlers.
      server->SetCommandHandler( "AUTH", Authenticate );
      server->SetCommandHandler( "001", LoggedIn );
      server->SetCommandHandler( "PING", Pong );

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

