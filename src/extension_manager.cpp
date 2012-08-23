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



vector<Extension*> *ExtensionManager::GetExtensions()
{
  return &extensions;
}



Extension *ExtensionManager::GetExtensionByPath( string path )
{
  vector<Extension*>::iterator extit;
  for( extit = extensions.begin(); extit != extensions.end(); ++extit )
  {
    if( (*extit)->extensionPath.compare( path ) == 0 )
    {
      return (*extit);
    }
  }
  return NULL;
}



Extension *ExtensionManager::GetExtensionByName( string name )
{
  vector<Extension*>::iterator extit;
  for( extit = extensions.begin(); extit != extensions.end(); ++extit )
  {
    if( (*extit)->extensionName.compare( name ) == 0 )
    {
      return (*extit);
    }
  }
  return NULL;
}



bool ExtensionManager::Update()
{
  vector<string> files;

  GetFilesInDir( extensionDir, files );

  // Check for removed extensions
  bool found;
  string extLib;
  vector<Extension*>::iterator extit;
  vector<string>::iterator fit;
  unsigned int loop = 0;
  for( extit = extensions.begin(); extit != extensions.end(); ++loop )
  {
    if( loop >= extensions.size() || !(*extit) )
    {
      extit = extensions.begin();
      loop  = 0;
      continue;
    }
    extLib = (*extit)->extensionLib;
    found = false;
    for( fit = files.begin(); fit != files.end(); ++fit )
    {
      if( extLib.compare( (*fit) ) == 0 )
      {
        found = true;
        break;
      }
    }
    if( !found )
    {
      UnloadExtension( (*extit)->extensionLib );
    }
    ++extit;
  }

  // Check existing and new extensions
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
  Extension *ext = GetExtensionByPath( extensionDir+libname );
  string path = extensionDir;
  path.append( libname );
  if( ext )
  {
    if( ext->extensionTimestamp >= GetTimestamp( path.c_str() ) )
    {
      return true;
    }

    printf( "Reloading updated extension '%s'..\n", libname.c_str() );
    UnloadExtension( libname );
  }
  LoadExtension( libname );
  return true;
}



bool ExtensionManager::LoadExtension( string libname )
{
  Extension *extension = new Extension;
  extension->extensionPath = extensionDir+libname;
  extension->extensionLib  = libname;
  extension->extensionHandle = dlopen( extension->extensionPath.c_str(), RTLD_NOW );
  if( !extension->extensionHandle )
  {
    fprintf( stderr, "Failed to load extension library '%s'! - %s\n", libname.c_str(), dlerror() );
    return false;
  }
  
  extension->CreateExtension = (ExtensionBase*(*)())dlsym( extension->extensionHandle, "CreateExtension" );
  if( !extension->CreateExtension )
  {
    fprintf( stderr, "Failed to load CreateExtension from library '%s'!\n", libname.c_str() );
    dlclose( extension->extensionHandle );
    return false;
  }

  extension->DestroyExtension = (void*(*)(ExtensionBase*))dlsym( extension->extensionHandle, "CreateExtension" );
  if( !extension->DestroyExtension )
  {
    fprintf( stderr, "Failed to load DestroyExtension from library '%s'!\n", libname.c_str() );
    dlclose( extension->extensionHandle );
    return false;
  }
  
  printf( "Finished loading extension library '%s'.\n", libname.c_str() );

  extension->extensionClass = extension->CreateExtension();
  if( !extension->DestroyExtension )
  {
    fprintf( stderr, "Failed to initialize the extensionClass from library '%s'!\n", libname.c_str() );
    dlclose( extension->extensionHandle );
    return false;
  }

  extension->extensionTimestamp = GetTimestamp( extension->extensionPath.c_str() );

  extension->extensionName = extension->extensionClass->GetName();

  extension->extensionClass->SetExtensionManager( (void*)this );

  printf( "The name of the loaded extension was '%s'\n", extension->extensionName.c_str() );


  // Push the extension to the vector
  extensions.push_back( extension );
  return true;
}



bool ExtensionManager::UnloadExtension( string libname )
{
  vector<Extension*>::iterator extit;
  for( extit = extensions.begin(); extit != extensions.end(); )
  {
    if( (*extit) )
    {
      if( (*extit)->extensionPath.compare( extensionDir+libname ) == 0 )
      {
        string name = (*extit)->extensionName;
        if( (*extit)->extensionClass )
        {
          (*extit)->DestroyExtension( (*extit)->extensionClass );
          (*extit)->extensionClass = NULL;
          dlclose( (*extit)->extensionHandle );
          (*extit)->extensionHandle = NULL;
        }

        (*extit)->extensionClass = NULL;
        extit = extensions.erase( extit );
        printf( "Unloaded extension '%s'.\n", name.c_str() );
        continue;
      }
    }
    ++extit;
  }
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

  while( (dirp = readdir(dp)) != NULL )
  {

      files.push_back( string( dirp->d_name ) );
  }

  closedir( dp );

  return true;
}



bool ExtensionManager::HandleCommands( Server *server )
{
  vector<Command> *commands = new vector<Command>();
  server->GetCommands( commands );

  vector<Extension*>::iterator extit;
  for( extit = extensions.begin(); extit != extensions.end(); ++extit )
  {
    (*extit)->extensionClass->HandleCommands( server, commands );
  }

  delete commands;

  return true;
}

