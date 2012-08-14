#include "extension.hpp"

using std::map;
using std::vector;
using std::string;

bool ExtensionBase::HandleCommands( vector<CommandHandler> *commands )
{
  name = "ExtensionBase";
  vector<Command>::iterator comIt;
  for( comIt = commands->begin(); comIt != commands->end(); ++comIt )
  {
    char *cmd = (*comIt).command;
    CommandHandler handler = callbackMap[cmd];
    if( handler )
    {
      printf( "Extension handling command %s\n", (*comIt).command );
      handler( (*comIt), this );
    }
  }

  return true;
}



bool ExtensionBase::SetCommandHandler( string key, CommandHandler handler )
{
  callbackMap.erase( key );
  callbackMap.insert( std::pair<string, CommandHandler>( key, handler ) );
  return true;
}



string ExtensionBase::GetName()
{
  return name;
}

