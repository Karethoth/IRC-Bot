#include "server.hpp"
#include <cerrno>

using namespace IRC;
using std::string;
using std::vector;
using std::map;


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



/* Initializers */
Server::Server()
{
  bufferSize = 1024;
  state = NOT_CONNECTED;
}



Server::Server( string      host,
                int         port )
                : serverPort( port )
{
  Init( host, port, -1 );
}



void Server::Init( string host,
                   int port,
                   int sock )
{
  serverHost = host;
  serverPort = port;

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



bool Server::HandleCommands()
{
  vector<Command> *commands = GetCommands();

  vector<Command>::iterator comIt;
  for( comIt = commands->begin(); comIt != commands->end(); ++comIt )
  {
    char *cmd = (*comIt).command;
    CommandHandler handler = callbackMap[cmd];
    if( handler )
    {
      printf( "handling command %s\n", (*comIt).command );
      handler( (*comIt), this );
    }
    else
    {
      printf( "No handler for command %s was found.\n", (*comIt).command );
    }
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

  char *offset      = tmp;
  char *nextLineEnd = NULL;

  while( (nextLineEnd = strstr( offset, "\r\n" )) > 0 )
  {

    nextLineEnd[0] = 0;
    Command cmd = ParseCommand( offset );
    commands->push_back( cmd );
    offset = (nextLineEnd+2);
  }

  delete tmp;

  return commands;
}



bool Server::SetCommandHandler( string key, CommandHandler handler )
{
  callbackMap.erase( key );
  callbackMap.insert( std::pair<string, CommandHandler>( key, handler ) );
  return true;
}
