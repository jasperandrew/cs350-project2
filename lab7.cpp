#include <iostream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vector>
#include "lab7.h"

using namespace std;
using namespace boost::algorithm;

int main(int argc, char **argv)
{
    
  string command;
  vector<string> args; 
  getline(cin, command);
  split(args, command, is_any_of(" "));
  while(!command.empty())
    {
      if(command == "import")
	{
          cout << "importing file " <<  command << " lfs file\n";
          //import(arg1, arg2);
        }
      getline(cin, command, ' ');
    }
  
	
  return 0;
}

