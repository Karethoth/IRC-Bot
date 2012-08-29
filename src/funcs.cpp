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



