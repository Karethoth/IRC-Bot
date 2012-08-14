#include "extension_manager.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <dlfcn.h>
#include <cstring>

using std::vector;
using std::string;


bool GetFilesInDir( string dir, vector<string> &files );


ExtensionManager::ExtensionManager()
{
  char *extensionDirBuf = new char[256];
  memset( extensionDirBuf, 0, 256 );

  getcwd( extensionDirBuf, 255 );

  string extDir = string( extensionDirBuf );
  delete extensionDirBuf;

  extDir.append( "/extensions/" );

  extensionDir = extDir;
}



vector<Extension> *ExtensionManager::GetExtensions()
{
  return &extensions;
}



bool ExtensionManager::Update()
{
  vector<string> files;

  GetFilesInDir( extensionDir, files );

  vector<string>::iterator fit;
  for( fit = files.begin(); fit != files.end(); ++fit )
  {
    if( strstr( (*fit).c_str(), ".so" ) )
    {
      HandleExtension( (*fit) );
    }
  }
  return true;
}


bool ExtensionManager::HandleExtension( string libname )
{
  // For now, just load it.
  LoadExtension( libname );
}



bool ExtensionManager::LoadExtension( string libname )
{
  Extension extension;
  extension.extensionHandle = dlopen( extensionDir.append( libname ).c_str(), RTLD_NOW );
  if( !extension.extensionHandle )
  {
    fprintf( stderr, "Failed to load extension library '%s'!\n", libname.c_str() );
    return false;
  }
  printf( "Loaded extension library '%s'.\n", libname.c_str() );
  return true;
}



bool GetFilesInDir( string dir, vector<string> &files )
{
  DIR *dp;
  struct dirent *dirp;
  if( (dp  = opendir( dir.c_str() )) == NULL )
  {
      printf( "Couldn't open extensions-dir\n" );
      return false;
  }

  while ((dirp = readdir(dp)) != NULL) {
      files.push_back(string(dirp->d_name));
  }
  closedir(dp);
  return true;
}

