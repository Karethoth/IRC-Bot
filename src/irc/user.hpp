#ifndef __IRC_USER__
#define __IRC_USER__

namespace IRC
{
  struct User
  {
    int   id;
    char *nick;
    char *ident;
    char *realName;
    char *host;
  };
}

#endif

