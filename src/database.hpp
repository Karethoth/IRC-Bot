#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include <sqlite3.h>
#include <map>

extern bool DBExists( std::string path );
extern bool CreateDB( std::string path );

extern std::map<std::string, std::string> GetSettings( sqlite3 *db );
extern bool TableExists( sqlite3 *db, std::string table );

#endif

