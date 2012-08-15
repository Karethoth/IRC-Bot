/*
 * This is a very minimalistic example of how to
 * write an extension.
 * --------------
 * Of course, you're free to use multiple files
 * and classes, external libraries, etc~
 * --------------
 * You can also overwrite the virtual functions of
 * the ExtensionBase and exploit it mercilessly.
 */

#include "template.hpp"


// These functions are needed to make
// the extension loadable and removable.
extern "C"
{
  ExtensionBase *CreateExtension()
  {
    // Change TemplateExtension to the
    // main class of your extension.
    return new TemplateExtension;
  }

  void *DestroyExtension( ExtensionBase *extension )
  {
    delete extension;
  }
}



// Predefine the functions that
// are used by your extension.
bool Pong( IRC::Command, void* );



// Constructor of the main class
// of the extension
TemplateExtension::TemplateExtension()
{
  // Please, set the name of the extension
  // to make everything nice and tidy.
  name = "TemplateExtension";

  // Here the commands from irc are
  // "linked" to the actual functions.
  SetCommandHandler( "PING", Pong );
}



// And here's an example function. A ponger.
// You might want to familiarize yourself with
// the structures of IRC::Command and IRC::Server.
bool Pong( IRC::Command cmd, void *server )
{
  // We want to cast the void*server to something
  // a bit more usable, this does just that.
  IRC::Server *srv = static_cast<IRC::Server*>( server );

  // Basic C++
  std::string msg = "PONG :";
  msg.append( cmd.data );
  msg.append( "\r\n" );

  // IRC::Server::Write( std::string ) is
  // used to send raw data to the server.
  srv->Write( msg );

  // Extension can also change the state of the server.
  // Though, it's way more preferable to write your own
  // states to the main class and use them. This is NOT
  // recommended:
  srv->SetState( IRC::WORKING );

  return true;
}

