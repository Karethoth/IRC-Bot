#include "server.hpp"
#include <cerrno>

using namespace IRC;
using std::string;
using std::vector;
using std::map;



/* Initializer */
Server::Server( string      host,
                int         port )
                : serverPort( port )
{
  serverHost = host;

  bufferSize = 1024;
  buffer = new char[bufferSize];
  if( !buffer )
  {
    fprintf( stderr, "Error allocating servers buffer!\n" );
  }

  state = NOT_CONNECTED;
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
  if( write( sockfd, msg.c_str(), msg.length() ) <= -1 )
    return false;
  return true;
}



bool Server::HandleCommands()
{
  vector<Command> *commands = GetCommands();

  vector<Command>::iterator comIt;
  for( comIt = commands->begin(); comIt != commands->end(); ++comIt )
  {
    printf( "received command %s\n", (*comIt).command );
  }

  return true;
}



vector<Command> *Server::GetCommands()
{
  vector<Command> *commands = new vector<Command>();
  
  string data = "";

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

  char *pch;
  pch = strtok( tmp, "\n" );
  while( pch != NULL )
  {
    Command cmd = ParseCommand( pch );
    commands->push_back( cmd );
    pch = strtok( NULL, "\n" );
  }

  delete tmp;

  return commands;
}

