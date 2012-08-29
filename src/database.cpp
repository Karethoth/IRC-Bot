#include <unistd.h>
#include <sys/stat.h>
#include <sqlite3.h>
#include <iostream>
#include <map>

#include "funcs.hpp"

using namespace std;


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

  sqlite3_free( stmt );

  return settings;
}



bool TableExists( sqlite3 *db, string table )
{
  sqlite3_stmt *stmt;
  if( !db )
    return false;

  string q = "SELECT count(*) from sqlite_master where type='table' and name='";
  q.append( table );
  q.append( "'\n" );

  if( sqlite3_prepare_v2( db, q.c_str(), -1, &stmt, NULL ) ) // <-- Segfault if table exists
  {
    cerr << "DB err: " << sqlite3_errmsg( db ) << endl;
    return false;
  }

  bool exists = false;
  if( sqlite3_step( stmt ) == SQLITE_ROW )
  {
    if( sqlite3_column_int( stmt, 0 ) )
    {
      exists = true;
    }
  }

  sqlite3_free( stmt );
  return exists;
}

