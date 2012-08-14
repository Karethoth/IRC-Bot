#include "handlers.hpp"

using namespace IRC;


// Command handlers
bool Authenticate( Command, void *);
bool LoggedIn( Command, void *);
bool Pong( Command, void *);
bool Msg( Command, void *);


extern "C" bool SetCommandHandlers( Server *server )
{
  if( !server )
    return false;

  server->SetCommandHandler( "AUTH", Authenticate );
  server->SetCommandHandler( "001", LoggedIn );
  server->SetCommandHandler( "PING", Pong );
  server->SetCommandHandler( "PRIVMSG", Msg );

  return true;
}



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



bool Msg( IRC::Command cmd, void *server )
{
  IRC::Server *srv = static_cast<IRC::Server*>( server );
  printf( "<%s> %s\n", cmd.source, cmd.data );
  return true;
}

