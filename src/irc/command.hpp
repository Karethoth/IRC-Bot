#ifndef __IRC_COMMAND__
#define __IRC_COMMAND__

#include "user.hpp"

namespace IRC
{
  struct Command
  {
    char *source;
    char *target;
    char *command;
    char *data;
    char *raw;
    struct User *user;
  };

  Command ParseCommand( char *msg );
}

#endif

