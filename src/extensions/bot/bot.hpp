#ifndef __BOT_HPP__
#define __BOT_HPP__

#include "../extension.hpp"
#include "../../extension_manager.hpp"


class BotExtension : public ExtensionBase
{
 private:
  std::string botName;
  std::string botUsername;
  std::string botHost;
  std::string botRealname;

  // Custom
  ExtensionManager *extensionMan;

 protected:
  bool Msg( IRC::Command, IRC::Server* );
  bool Pong( IRC::Command, IRC::Server* );
  bool Authenticate( IRC::Command, IRC::Server* );
  bool LoggedIn( IRC::Command, IRC::Server* );
  bool HandleUserCommand( IRC::Command, IRC::User*, IRC::Server* );

 public:
  BotExtension();

  virtual void SetExtensionManager( void *extMan );

  virtual bool HandleCommands( IRC::Server *server, std::vector<IRC::Command> *commands );
};

#endif

