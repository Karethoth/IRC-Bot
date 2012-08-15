#ifndef __EXTENSION_MANAGER__
#define __EXTENSION_MANAGER__

#include <iostream>
#include <vector>
#include "extensions/extension.hpp"
#include "irc/irc.hpp"


struct Extension
{
  std::string    extensionName;
  std::string    extensionPath;
  int            extensionTimestamp;
  ExtensionBase *extensionClass;
  void          *extensionHandle;

  // Creater and destroyer
  ExtensionBase *(*CreateExtension)();
  void          *(*DestroyExtension)(ExtensionBase*);
};



class ExtensionManager
{
 private:
  std::string             extensionDir;
  std::vector<Extension*> extensions;

  bool HandleExtension( std::string libname );
  bool LoadExtension( std::string libname );
  bool UnloadExtension( std::string libname );

 public:
  ExtensionManager();

  bool Update();
  bool HandleCommands( IRC::Server *server );

  std::vector<Extension*> *GetExtensions();
  Extension *GetExtensionByName( std::string name );
  Extension *GetExtensionByPath( std::string path );
};

#endif

