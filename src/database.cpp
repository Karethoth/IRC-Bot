#include <unistd.h>
#include <sys/stat.h>
#include <sqlite3.h>
#include <iostream>

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
  int queryCount = 4;
  string queries[] =
  {
    "CREATE TABLE Settings( key varchar(30), value text )",
    "INSERT INTO Settings VALUES( \"nick\", \"_NICK_\" )",
    "INSERT INTO Settings VALUES( \"nickAlternative\", \"_ALTERNATIVE_\" )",
    "INSERT INTO Settings VALUES( \"serverHost\", \"_SERVERHOST_\" )",
  };

  string nick, nickAlternative, serverHost;

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

    ret = sqlite3_exec( db, q.c_str(), NULL, 0, &errMesg );
    if( ret != SQLITE_OK )
    {
      sqlite3_free( errMesg );
    }
  }



  sqlite3_close( db );

  return true;
}


