#ifndef __IRC_COMMAND__
#define __IRC_COMMAND__

namespace IRC
{
  struct Command
  {
    char *source;
    char *target;
    char *command;
    char *data;
    char *raw;
  };

  Command ParseCommand( char *msg );
}

#endif

