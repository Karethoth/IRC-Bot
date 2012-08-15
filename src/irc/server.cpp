#include "server.hpp"
#include "../extension_manager.hpp"

#include <cerrno>

using namespace IRC;
using std::string;
using std::vector;
using std::map;


/*
extern "C" {
  Server *ServerMaker()
  {
    return new Server();
  }

  void ServerDestroyer( Server *server )
  {
    delete server;
  }
}
*/


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
  struct sockaddr_in servAddr;
  struct hostent *server;

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

  state = NOT_CONNECTED;
  close( sockfd );
}



bool Server::Write( string msg )
{
  printf( "SENDING TO SERVER: '%s'\n", msg.c_str() );
  if( write( sockfd, msg.c_str(), msg.length() ) <= -1 )
    return false;
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
    if( commands )
    {
      commands->push_back( cmd );
    }
    offset = (nextLineEnd+2);
  }

  delete tmp;

  return true;
}



