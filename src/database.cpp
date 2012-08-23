#include <unistd.h>
#include <sys/stat.h>
#include <sqlite3.h>
#include <iostream>
#include <map>

using namespace std;


string QueryUser( string msg )
{
  cout << msg;
  string answer;
  cin >> answer;
  return answer;
}



string ReplaceSubstring( string str, const std::string &from, string &to )
{
  size_t start = str.find( from );

  if( start == std::string::npos )
      return str;

  str.replace( start, from.length(), to );
  return str;
}



bool DBExists( string path )
{
  struct stat st;
  if( stat( path.c_str(), &st ) == 0 )
    return true;

  return false;
}



bool CreateDB( string path )
{
  int queryCount = 5;
  string queries[] =
  {
    "CREATE TABLE Settings( key varchar(30), value text )",
    "INSERT INTO Settings VALUES( \"nick\", \"_NICK_\" )",
    "INSERT INTO Settings VALUES( \"nickAlternative\", \"_ALTERNATIVE_\" )",
    "INSERT INTO Settings VALUES( \"serverHost\", \"_SERVERHOST_\" )",
    "INSERT INTO Settings VALUES( \"serverPort\", \"_SERVERPORT_\" )",
  };

  string nick, nickAlternative, serverHost, serverPort;

  while( nick.length() <= 0 )
  {
    nick = QueryUser( "Nickname: " );
  }

  while( nickAlternative.length() <= 0 )
  {
    nickAlternative = QueryUser( "Alternative name: " );
  }

  while( serverHost.length() <= 0 )
  {
    serverHost = QueryUser( "Server host: " );
  }

  while( serverPort.length() <= 0 )
  {
    serverPort = QueryUser( "Server port: " );
  }


  sqlite3 *db;
  char *errMesg = NULL;

  int ret = sqlite3_open( path.c_str(), &db );
  if( ret )
  {
    return false;
  }


  string q;
  for( int i=0; i < queryCount; ++i )
  {
    q = queries[i];
    q = ReplaceSubstring( q, "_NICK_", nick );
    q = ReplaceSubstring( q, "_ALTERNATIVE_", nickAlternative );
    q = ReplaceSubstring( q, "_SERVERHOST_", serverHost );
    q = ReplaceSubstring( q, "_SERVERPORT_", serverPort );

    ret = sqlite3_exec( db, q.c_str(), NULL, 0, &errMesg );
    if( ret != SQLITE_OK )
    {
      cerr << "DB err: " << sqlite3_errmsg( db ) << endl;
      sqlite3_free( errMesg );
    }
  }



  sqlite3_close( db );

  return true;
}


map<string, string> GetSettings( sqlite3 *db )
{
  map<string, string> settings = map<string, string>();

  sqlite3_stmt *stmt;
  //char *errMsg = 0;
  const char *select = "SELECT * FROM Settings";

  if( sqlite3_prepare_v2( db, select, -1, &stmt, NULL ) )
  {
    cerr << "DB err: " << sqlite3_errmsg( db ) << endl;
    return settings;
  }

  string key, val;
  while( sqlite3_step( stmt ) == SQLITE_ROW )
  {
    key = string( (const char*)sqlite3_column_text( stmt, 0 ) );
    val = string( (const char*)sqlite3_column_text( stmt, 1 ) );
    settings.insert( pair<string, string>( key, val ) );
  }

  return settings;
}

