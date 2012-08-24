#include "bot.hpp"

using namespace std;


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

    else if( !string("NICK").compare( (*comIt).command ) )
    {
      Nick( (*comIt), server );
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
    InitSettings( server );

    printf( "Setting nick\n" );
    tmp = "NICK ";
    tmp.append( server->GetServerSettings()["nick"] );
    tmp.append( "\r\n" );
    server->Write( tmp );
    server->SetState( IRC::SETTING_USER );
  }
  else if( server->GetState() == IRC::SETTING_USER )
  {
    printf( "Setting user\n" );
    tmp = "USER ";
    tmp.append( server->GetServerSettings()["nick"] );
    tmp.append( " " );
    tmp.append( server->GetServerSettings()["nick"] );
    tmp.append( " 8 :" );
    tmp.append( server->GetServerSettings()["nick"] );
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
  if( botNick.compare( user->nick ) == 0 )
  {
    target = string( cmd.target );
  }
  else
  {
    target = user->nick;
  }

  bool admin = IsAdmin( server, string( user->host ) );

  string reply = "PRIVMSG ";
  reply.append( target );
  reply.append( " :" );


  p = strstr( raw, " " );
  if( p )
  {
    p[0] = 0;
    para = string( p+1 );
  }
  else
  {
    para = "";
  }

  command  = string( raw );


  if( admin && command.compare( "join" ) == 0 )
  {
    server->Join( para );
  }
  else if( admin && command.compare( "part" ) == 0 )
  {
    server->Part( para );
  }
  else if( admin && command.compare( "nick" ) == 0 )
  {
    server->Nick( para );
  }
  else if( admin && command.compare( "settings" ) == 0 &&
           para.compare( "save" ) == 0 )
  {
    server->SetSettings( settings );
    server->SaveSettings();
  }
  else if( admin && command.compare( "settings" ) == 0 &&
           para.compare( "reload" ) == 0 )
  {
    ReloadSettings( server );
  }
  else if( admin && command.compare( "addadmin" ) == 0 )
           
  {
    AddAdmin( server, para );
  }
  else if( admin && command.compare( "list" ) == 0 &&
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
    server->Write( reply + "Not a valid command or no rights!\r\n" );
  }

  delete user;

  return true;
}



bool BotExtension::InitSettings( IRC::Server *server )
{
  server->ReloadSettings();
  settings = server->GetServerSettings();

  if( !TableExists( server->GetDB(), "BOT_Admins" ) )
  {
    SetupDB( server );
  }

  botNick = settings["nick"];

  return true;
}



bool BotExtension::ReloadSettings( IRC::Server *server )
{
  server->ReloadSettings();
  settings = server->GetServerSettings();

  if( settings["nick"].compare( botNick ) )
  {
    server->Nick( settings["nick"] );
  }

  return true;
}



bool BotExtension::Nick( IRC::Command cmd, IRC::Server *server )
{
  if( !botNick.compare( cmd.user->nick ) )
  {
    botNick = string( cmd.target+1 );
    settings["nick"] = botNick;
    server->SetSettings( settings );
    cout << "Changed nick to " << botNick << endl;
  }

  return true;
}



bool BotExtension::SetupDB( IRC::Server *server )
{
  sqlite3 *db = server->GetDB();
  cout << "Seems like it's the first time you're running bot extension.\n";
  cout << "Let's create an admin account for you.\n";

  server->DBExec( "CREATE TABLE BOT_Admins(identificaton varchar(50))" );

  string identification = "";
  while( identification.length() <= 0 )
  {
    identification = QueryUser( "A string that matches your user: " );
  }
  
  AddAdmin( server, identification );
  return true;
}



bool BotExtension::IsAdmin( IRC::Server *server, std::string host )
{
  string q = "SELECT * FROM BOT_Admins";
  sqlite3 *db = server->GetDB();
  sqlite3_stmt *stmt;

  bool admin = false;
  if( sqlite3_prepare_v2( db, q.c_str(), -1, &stmt, NULL ) )
  {
    cerr << "DB err: " << sqlite3_errmsg( db ) << endl;
    return admin;
  }

  string ident;
  while( sqlite3_step( stmt ) == SQLITE_ROW )
  {
    ident = string( (const char*)sqlite3_column_text( stmt, 0 ) );
    if( host.find( ident ) != string::npos )
    {
      admin = true;
      break;
    }
  }

  sqlite3_free( stmt );
  return admin;
}


bool BotExtension::AddAdmin( IRC::Server *server, std::string identification )
{
  string q = "INSERT INTO BOT_Admins VALUES ('";
  q.append( identification );
  q.append( "')" );
  server->DBExec( q );
  return true;
}

