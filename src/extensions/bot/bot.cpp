#include "bot.hpp"

using std::string;
using std::vector;


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


BotExtension::BotExtension()
{
  name = "Bot";
}



void BotExtension::SetExtensionManager( void *extMan )
{
  extensionMan = static_cast<ExtensionManager*>( extMan );
}



bool BotExtension::HandleCommands( IRC::Server *server, vector<IRC::Command> *commands )
{
  vector<IRC::Command>::iterator comIt;
  for( comIt = commands->begin(); comIt != commands->end(); ++comIt )
  {
    if( !string("PRIVMSG").compare( (*comIt).command ) )
    {
      Msg( (*comIt), server );
    }

    else if( !string("PING").compare( (*comIt).command ) )
    {
      Pong( (*comIt), server );
    }

    else if( !string("AUTH").compare( (*comIt).command ) )
    {
      Authenticate( (*comIt), server );
    }

    else if( !string("NOTICE").compare( (*comIt).command ) &&
             (server->GetState() == IRC::SETTING_NICK ||
              server->GetState() == IRC::SETTING_USER ) &&
             (strstr( (*comIt).data, "AUTH" ) ||
              strstr( (*comIt).data, "Ident" ) ) )
    {
      Authenticate( (*comIt), server );
    }

    else if( !string("001").compare( (*comIt).command ) )
    {
      LoggedIn( (*comIt), server );
    }
  }

  return true;
}



bool BotExtension::Authenticate( IRC::Command cmd, IRC::Server *server )
{
  if( server->GetState() == IRC::SETTING_NICK )
  {
    printf( "Setting nick\r\n" );
    server->Write( "NICK KoukariBot\n" );
    server->SetState( IRC::SETTING_USER );
  }
  else if( server->GetState() == IRC::SETTING_USER )
  {
    printf( "Setting user\r\n" );
    server->Write( "USER meh meh meh 8:KoukariBot\r\n" );
  }
  return true;
}



bool BotExtension::LoggedIn( IRC::Command cmd, IRC::Server *server )
{
  if( string( cmd.command ).compare("001") )
  {
    server->SetState( IRC::WORKING );
  }
}



bool BotExtension::Pong( IRC::Command cmd, IRC::Server *server )
{
  string msg = "PONG :";
  msg.append( cmd.data );
  msg.append( "\r\n" );
  server->Write( msg );
  server->SetState( IRC::WORKING );
  return true;
}



bool BotExtension::Msg( IRC::Command cmd, IRC::Server *server )
{
  printf( "<%s> %s\n", cmd.source, cmd.data );

  if( strstr( cmd.data, "list extensions" ) )
  {
    if( !extensionMan )
    {
      printf( "extensionMan not set!\n" );
      return true;
    }
    printf( "list of extensions:\n" );

    vector<Extension*> *extensions = extensionMan->GetExtensions();
    vector<Extension*>::iterator extit;
    for( extit = extensions->begin(); extit != extensions->end(); ++extit )
    {
      printf( "\t- %s\n", (*extit)->extensionName.c_str() );
    }
  }
  return true;
}

