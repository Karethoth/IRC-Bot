#include "bot.hpp"


extern "C"
{
  ExtensionBase *CreateExtension()
  {
    return new BotExtension;
  }

  void *DestroyExtension( ExtensionBase *extension )
  {
    delete extension;
  }
}



bool Authenticate( IRC::Command, void* );
bool LoggedIn( IRC::Command, void* );
bool Pong( IRC::Command, void* );
bool Msg( IRC::Command, void* );



BotExtension::BotExtension()
{
  name = "Bot";
  SetCommandHandler( "AUTH", Authenticate );
  SetCommandHandler( "001",  LoggedIn );
  SetCommandHandler( "PING", Pong );
  SetCommandHandler( "PRIVMSG", Msg );
}




bool Authenticate( IRC::Command cmd, void *server )
{
  IRC::Server *srv = static_cast<IRC::Server*>( server );
  if( srv->GetState() == IRC::SETTING_NICK )
  {
    printf( "Setting nick\r\n" );
    srv->Write( "NICK KoukariBot\n" );
    srv->SetState( IRC::SETTING_USER );
  }
  else if( srv->GetState() == IRC::SETTING_USER )
  {
    printf( "Setting user\r\n" );
    srv->Write( "USER meh meh meh 8:KoukariBot\r\n" );
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
  msg.append( "\r\n" );
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

