#include "command.hpp"
#include <cstring>
#include <cstdio>

using IRC::Command;


Command IRC::ParseCommand( char *msg )
{
  Command cmd;
  cmd.raw = new char[strlen( msg )];
  strcpy( cmd.raw, msg );
  cmd.source  = strtok( cmd.raw, " :" );
  cmd.command = strtok( 0, " " );
  cmd.target  = strtok( 0, " " );
  cmd.data    = strtok( 0, ":" );
  return cmd;
}

