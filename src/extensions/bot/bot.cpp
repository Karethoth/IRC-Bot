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
  name        = "Bot";
  botName     = "kariSlave";
  botUsername = "bot";
  botHost     = "home.ndirt.com";
  botRealname = "KoukariBot";
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
  string tmp;
  if( server->GetState() == IRC::SETTING_NICK )
  {
    printf( "Setting nick\n" );
    tmp = "NICK ";
    tmp.append( botName );
    tmp.append( "\r\n" );
    server->Write( tmp );
    server->SetState( IRC::SETTING_USER );
  }
  else if( server->GetState() == IRC::SETTING_USER )
  {
    printf( "Setting user\n" );
    tmp = "USER ";
    tmp.append( botUsername );
    tmp.append( " " );
    tmp.append( botHost );
    tmp.append( " 8 :" );
    tmp.append( botRealname );
    tmp.append( "\r\n" );
    server->Write( tmp );
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
  botName = server->GetUserInfo().nick;

  string msg = "PONG :";
  msg.append( cmd.data );
  msg.append( "\r\n" );
  server->Write( msg );
  server->SetState( IRC::WORKING );
  return true;
}



bool BotExtension::Msg( IRC::Command cmd, IRC::Server *server )
{
  printf( "<%s> to <%s> %s\n", cmd.user->nick, cmd.target, cmd.data );

  if( cmd.data[0] == '!' )
  {
    ++cmd.data;
    HandleUserCommand( cmd, cmd.user, server );
    return true;
  }

  return true;
}



bool BotExtension::HandleUserCommand( IRC::Command cmd, IRC::User *user, IRC::Server *server )
{
  string command;
  string para;
  string tmp;
  char *raw = cmd.data;
  char *params  = NULL;
  char *p;


  string target;
  if( botName.compare( user->nick ) == 0 )
  {
    target = string( cmd.target );
  }
  else
  {
    target = user->nick;
  }


  string reply = "PRIVMSG ";
  reply.append( target );
  reply.append( " :" );


  p = strstr( raw, " " );
  if( !p )
  {
    server->Write( reply + "Not a valid command!\r\n" );
    delete user;
    return true;
  }

  p[0] = 0;


  command  = string( raw );
  para     = string( p+1 );

  if( command.compare( "join" ) == 0 )
  {
    server->Join( para );
  }
  else if( command.compare( "part" ) == 0 )
  {
    server->Part( para );
  }
  else if( command.compare( "nick" ) == 0 )
  {
    server->Nick( para );
    botName = para;
  }
  else if( command.compare( "list" ) == 0 &&
           para.compare( "extensions" ) == 0 )
  {
    if( !extensionMan )
    {
      server->Write( reply+string( "Extension Manager not set! Sorry, can't list the extensions.\n" ) );
      printf( "extensionMan not set!\n" );
      return true;
    }


    server->Write( reply+string( "List of extensions:\n" ) );

    vector<Extension*> *extensions = extensionMan->GetExtensions();

    vector<Extension*>::iterator extit;
    for( extit = extensions->begin(); extit != extensions->end(); ++extit )
    {
      server->Write( reply+string( " - " )+(*extit)->extensionName+string( "\r\n" ) );
    }
  }
  else
  {
    server->Write( reply + "Not a valid command!\r\n" );
  }

  delete user;

  return true;
}

