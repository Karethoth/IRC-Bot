#include "extension_manager.hpp"
#include "extensions/extension.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <dlfcn.h>
#include <cstring>


using namespace IRC;
using std::vector;
using std::string;


bool GetFilesInDir( string dir, vector<string> &files );
extern int GetTimestamp( const char* ); // A quick hack


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
  // For now, just load it without checking it.
  LoadExtension( libname );
  return true;
}



bool ExtensionManager::LoadExtension( string libname )
{
  Extension extension;
  extension.extensionPath = extensionDir.append( libname );
  extension.extensionHandle = dlopen( extension.extensionPath.c_str(), RTLD_NOW );
  if( !extension.extensionHandle )
  {
    fprintf( stderr, "Failed to load extension library '%s'!\n", libname.c_str() );
    return false;
  }

  printf( "Loading library functions for '%s'....\n", libname.c_str() );
  
  extension.CreateExtension = (ExtensionBase*(*)())dlsym( extension.extensionHandle, "CreateExtension" );
  if( !extension.CreateExtension )
  {
    fprintf( stderr, "Failed to load CreateExtension from library '%s'!\n", libname.c_str() );
    dlclose( extension.extensionHandle );
    return false;
  }
  printf( "Loaded CreateExtension from library '%s'.\n", libname.c_str() );

  extension.DestroyExtension = (void*(*)(ExtensionBase*))dlsym( extension.extensionHandle, "CreateExtension" );
  if( !extension.DestroyExtension )
  {
    fprintf( stderr, "Failed to load DestroyExtension from library '%s'!\n", libname.c_str() );
    dlclose( extension.extensionHandle );
    return false;
  }
  printf( "Loaded DestroyExtension from library '%s'.\n", libname.c_str() );
  
  printf( "Finished loading extension library '%s'.\n", libname.c_str() );

  extension.extensionClass = extension.CreateExtension();
  if( !extension.DestroyExtension )
  {
    fprintf( stderr, "Failed to initialize the extensionClass from library '%s'!\n", libname.c_str() );
    dlclose( extension.extensionHandle );
    return false;
  }
  printf( "Initialized the extensionClass from library '%s'!\n", libname.c_str() );

  extension.extensionTimestamp = GetTimestamp( extension.extensionPath.c_str() );

  extension.extensionName = extension.extensionClass->GetName();
  printf( "The name of the loaded extension was '%s'\n", extension.extensionName.c_str() );


  // Push the extension to the vector
  extensions.push_back( extension );
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



bool ExtensionManager::HandleCommands( Server *server )
{
  vector<Command> *commands = server->GetCommands();

  vector<Extension>::iterator extit;
  for( extit = extensions.begin(); extit != extensions.end(); ++extit )
  {
    (*extit).extensionClass->HandleCommands( server, commands );
  }
  return true;
}

