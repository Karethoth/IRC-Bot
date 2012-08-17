#include "logger.hpp"

#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;


extern "C"
{
  ExtensionBase *CreateExtension()
  {
    return new LoggerExtension;
  }

  void *DestroyExtension( ExtensionBase *extension )
  {
    delete extension;
  }
}


bool Log( IRC::Command, void* );
bool MsgHandler( IRC::Command, void* );



LoggerExtension::LoggerExtension()
{
  name = "Logger";

  SetCommandHandler( "PRIVMSG", MsgHandler );
  SetCommandHandler( "JOIN", Log );
  SetCommandHandler( "PART", Log );
}



bool MsgHandler( IRC::Command cmd, void *server )
{
  //IRC::Server *srv = static_cast<IRC::Server*>( server );

  if( cmd.target[0] != '#' )
  {
    return true;
  }

  Log( cmd, server );

  return true;
}


string GenerateTimestamp()
{
  time_t t = time( 0 );
  struct tm *now = localtime( &t );

  char buff[11];
  sprintf( buff, "[%02d:%02d:%02d]",
           now->tm_hour,
           now->tm_min,
           now->tm_sec );

  return string( buff );
}


bool Log( IRC::Command cmd, void *server )
{
  time_t t = time( 0 );
  struct tm *now = localtime( &t );

  char filename[128];
  sprintf( filename, "logs/%s-%d-%02d-%02d.log", cmd.target, now->tm_year+1900, now->tm_mon+1, now->tm_mday );

  ofstream outf;
  outf.open( filename, ios::out | ios::app );

  if( string(cmd.command).compare( "PRIVMSG" ) == 0 )
  {
    outf << GenerateTimestamp() <<  "\t<" << cmd.user->nick << "> " << cmd.data << "\n";
  }
  else if( string(cmd.command).compare( "JOIN" ) == 0 )
  {
    outf << GenerateTimestamp() <<  "\t-> " << cmd.user->nick << " joined the channel.\n";
  }
  else if( string(cmd.command).compare( "PART" ) == 0 )
  {
    outf << GenerateTimestamp() <<  "\t<- " << cmd.user->nick << " has left the channel (" << (cmd.data? cmd.data : "") << ").\n";
  }

  outf.close();
  return true;
}

