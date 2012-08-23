#include "server.hpp"
#include "../extension_manager.hpp"

#include <cerrno>

using namespace IRC;
using std::string;
using std::vector;
using std::map;



/* Initializers */
Server::Server()
{
  bufferSize = 1024;
  state = NOT_CONNECTED;
  extensionManager = NULL;
}



Server::Server( string      host,
                int         port )
{
  Init( host, port, -1 );
}



void Server::Init( string host,
                   int port,
                   int sock )
{
  serverHost = host;
  serverPort = port;

  extensionManager = NULL;

  bufferSize = 1024;
  buffer = new char[bufferSize];
  if( !buffer )
  {
    fprintf( stderr, "Error allocating servers buffer!\n" );
  }

  state = NOT_CONNECTED;
  sockfd = sock;
}


/* Destructor */
Server::~Server()
{
  delete buffer;
}



/* Basic methods */
bool Server::Connect()
{
  userInfo.oldNick = "nonick";
  struct sockaddr_in servAddr;
  struct hostent *server;

  printf( "Connecting..\n" );

  sockfd = socket( AF_INET, SOCK_STREAM, 0 );
  if( sockfd < 0 )
  {
    fprintf( stderr, "Error opening socket\n" );
    return false;
  }

  server = gethostbyname( serverHost.c_str() );
  if( !server )
  {
    fprintf( stderr, "Error getting host\n" );
    return false;
  }

  memset( (void*)&servAddr, 0, sizeof( servAddr ) );
  servAddr.sin_family = AF_INET;
  servAddr.sin_port = htons( serverPort );
  servAddr.sin_addr.s_addr = ((struct in_addr*)(server->h_addr))->s_addr;

  if( connect( sockfd, (struct sockaddr*)&servAddr, sizeof( servAddr )) < 0 )
  {
    fprintf( stderr, "Error connecting %d\n", errno );
    return false;
  }

  printf( "Connected!\n" );

  state = SETTING_NICK;
  return true;
}



void Server::Disconnect()
{
  if( !IsConnected() )
    return;

  Write( "QUIT\n" );

  state = NOT_CONNECTED;
  close( sockfd );
}



bool Server::Write( string msg )
{
  //printf( "SENDING TO SERVER: '%s'\n", msg.c_str() );
  if( write( sockfd, msg.c_str(), msg.length() ) <= -1 )
    return false;
  return true;
}



bool Server::Join( string channel )
{
  Write( string("JOIN ")+channel+string("\r\n") );
  channels.push_back( channel );
  return true;
}



bool Server::Part( string channel )
{
  Write( string("PART ")+channel+string("\r\n") );
  vector<string>::iterator chanit;
  for( chanit = channels.begin(); chanit != channels.end(); )
  {
    if( (*chanit).compare( channel ) == 0 )
    {
      chanit = channels.erase( chanit );
      continue;
    }
    ++chanit;
  }
  return true;
}



bool Server::Nick( string newNick )
{
  Write( string("NICK ")+newNick+string("\r\n") );
  userInfo.oldNick = userInfo.nick;
  userInfo.nick    = newNick;
  return true;
}



bool Server::GetCommands( vector<Command> *commands )
{
  string data = "";

  commands->clear();

  ssize_t n;
  do
  {
    memset( buffer, 0, bufferSize );
    n = read( sockfd, buffer, bufferSize );
    if( n <= 0 )
    {
      state = NOT_CONNECTED;
      return false;
    }
    data.append( buffer, n );
  }
  while( n == bufferSize );

  char *tmp = new char[data.length()];
  data.copy( tmp, data.length(), 0 );

  char *offset      = tmp;
  char *nextLineEnd = NULL;

  while( (nextLineEnd = strstr( offset, "\r\n" )) > 0 )
  {

    nextLineEnd[0] = 0;
    Command cmd = ParseCommand( offset );

    if( cmd.command != NULL )
      commands->push_back( cmd );

    offset = (nextLineEnd+2);
  }

  delete tmp;

  return true;
}



