#include "user.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>


IRC::User *IRC::ParseUser( char *source )
{
  if( source[0] != ':' )
  {
    fprintf( stderr, "Bad source to parse: '%s'!\n", source );
    return NULL;
  }

  struct IRC::User *usr = new IRC::User;
  usr->raw = source+1;
  
  usr->nick = usr->raw;
  char *p = strstr( usr->raw, "!" );

  if( !p )
  {
    usr->ident = NULL;
    usr->host = NULL;
    return usr;
  }

  p[0] = 0;

  usr->ident = p+1;
  p = strstr( usr->ident, "@" );
  p[0] = 0;

  usr->host = p+1;

  return usr;
}

