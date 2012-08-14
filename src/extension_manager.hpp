#ifndef __EXTENSION_MANAGER__
#define __EXTENSION_MANAGER__

#include <iostream>
#include <vector>
#include "extensions/extension.hpp"


struct Extension
{
  std::string    extensionPath;
  int            extensionTimestamp;
  ExtensionBase *extensionClass;
  void          *extensionHandle;
};


class ExtensionManager
{
 private:
  std::string            extensionDir;
  std::vector<Extension> extensions;

  bool HandleExtension( std::string libname );
  bool LoadExtension( std::string libname );

 public:
  ExtensionManager();
  std::vector<Extension> *GetExtensions();
  bool Update();
};

#endif

