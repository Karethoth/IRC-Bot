#include "command.hpp"
#include <string>
#include <cstring>
#include <cstdio>

using IRC::Command;


Command IRC::ParseCommand( char *msg )
{
  printf( "Message: '%s'\n", msg );
  Command cmd;
  cmd.raw = new char[strlen( msg )+1];
  strcpy( cmd.raw, msg );
  cmd.raw[strlen( msg )+1] = 0;

  cmd.source = cmd.raw;

  char *spacePointer = strstr( cmd.raw, " " );

  spacePointer[0]=0;
  ++spacePointer;

  cmd.command = spacePointer;

  spacePointer = strstr( spacePointer, " " );

  if( spacePointer == NULL )
  {
    cmd.data    = cmd.command+1;
    cmd.command = cmd.source;
    return cmd;
  }

  spacePointer[0]=0;

  if( std::string( cmd.source ).compare( "ERROR" ) == 0 )
  {
    cmd.data    = cmd.command+1;
    cmd.command = cmd.source;
    return cmd;
  }

  ++spacePointer;

  if( std::string( cmd.command ).compare( "372" ) == 0 )
    return cmd;

  cmd.target = spacePointer;

  spacePointer    = strstr( spacePointer, " " );
  spacePointer[0] = 0;
  ++spacePointer;

  cmd.data = spacePointer;

  if( cmd.data[0] == ':' )
    ++cmd.data;

  return cmd;
}

