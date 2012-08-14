#include <dlfcn.h>
#include <sys/stat.h>
#include <iostream>
#include "irc/irc.hpp"

/*

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

*/

const char *libircPath = "/opt/lib/libirc.so";


int TimeChanged( const char *path )
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
  void *libircHandle  = NULL;
  int   libircChanged = 0;
  int   libircUpdated = 0;

  while( true )
  {
    // Load libirc if it has been updated.
    updated = TimeChanged( libircPath );
    if( libircChanged < updated )
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

      printf( "irclib loaded.\n" );

      libircChanged = updated;
    }
  }

  /*
  IRC::Server *is = new IRC::Server( "irc.quakenet.org", 6667 );

  is->SetCommandHandler( "AUTH", Authenticate );
  is->SetCommandHandler( "001", LoggedIn );
  is->SetCommandHandler( "PING", Pong );

  if( !is->Connect() )
    return -1;

  while( is->IsConnected() )
  {
    is->HandleCommands();
  }
  is->Disconnect();
  delete is;
  */

  return 0;
}

