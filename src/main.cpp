#include "irc/irc.hpp"


int main()
{
  IRC::Server *is = new IRC::Server( "irc.quakenet.org", 6667 );
  if( !is->Connect() )
    return -1;
  is->HandleCommands();
  is->Disconnect();
  delete is;

  return 0;
}
