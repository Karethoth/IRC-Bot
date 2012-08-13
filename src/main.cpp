#include "irc/irc.hpp"

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



int main()
{
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

  return 0;
}
