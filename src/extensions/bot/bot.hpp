#ifndef __BOT_HPP__
#define __BOT_HPP__

#include "../extension.hpp"
#include "../../extension_manager.hpp"
#include "../../funcs.hpp"


class BotExtension : public ExtensionBase
{
 private:
  std::string botNick;
  std::string botUsername;
  std::string botHost;
  std::string botRealname;

  std::map<std::string,std::string> settings;

  // Custom
  ExtensionManager *extensionMan;

 protected:
  bool Msg( IRC::Command, IRC::Server* );
  bool Pong( IRC::Command, IRC::Server* );
  bool Nick( IRC::Command, IRC::Server* );
  bool Authenticate( IRC::Command, IRC::Server* );
  bool LoggedIn( IRC::Command, IRC::Server* );
  bool HandleUserCommand( IRC::Command, IRC::User*, IRC::Server* );

  bool InitSettings( IRC::Server* );
  bool ReloadSettings( IRC::Server* );

  bool SetupDB( IRC::Server* );

  bool IsAdmin( IRC::Server*, std::string );
  bool AddAdmin( IRC::Server*, std::string );

 public:
  BotExtension();

  virtual void SetExtensionManager( void *extMan );

  virtual bool HandleCommands( IRC::Server *server, std::vector<IRC::Command> *commands );
};

#endif

