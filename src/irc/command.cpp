#include "command.hpp"
#include <string>
#include <cstring>
#include <cstdio>

using IRC::Command;


Command IRC::ParseCommand( char *msg )
{
  Command cmd;
  cmd.raw = new char[strlen( msg )];
  strcpy( cmd.raw, msg );
  cmd.source  = strtok( cmd.raw, " :" );

  // Check if we're handling a PING request
  if( std::string( cmd.source ).compare("PING") == 0 )
  {
    cmd.command = cmd.source;
    cmd.data    = strtok( 0, ":" );
    return cmd;
  }
  cmd.command = strtok( 0, " " );
  cmd.target  = strtok( 0, " " );
  cmd.data    = strtok( 0, ":" );
  return cmd;
}

