#ifndef __EXTENSION_HPP__
#define __EXTENSION_HPP__

#include <map>
#include <vector>

#include "../irc/irc.hpp"


class ExtensionBase
{
 protected:
  std::string name;
  std::map<std::string, CommandHandler> callbackMap;

 public:
  ExtensionBase()
  {
    name = "ExtensionBase";
  }


  virtual bool HandleCommands( std::vector<IRC::Command>* )
  {
    std::vector<Command>::iterator comIt;
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


  virtual bool SetCommandHandler( std::string key, CommandHandler handler )
  {
    callbackMap.erase( key );
    callbackMap.insert( std::pair<string, CommandHandler>( key, handler ) );
    return true;
  }


  virtual std::string GetName()
  {
    return name;
  }
};

#endif

