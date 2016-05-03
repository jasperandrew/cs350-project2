#include "lab7.h"

int main(int argc, char **argv)
{
  //argument parsing
  string command;
  vector<string> args;
  while(getline(cin, command) && !command.empty())
    {     
      split(args, command, is_any_of(" "));
      if(command == "import")
        {
	  cout << "importing file " <<  command << " lfs file\n";
	  //import(arg1, arg2);                                                 
        }
      else if(command == "import")
        {
          cout << "importing file " <<  command << " lfs file\n";
          //import(arg1, arg2);                                                                                                                                                    
        }
      else if(command == "import")
        {
          cout << "importing file " <<  command << " lfs file\n";
          //import(arg1, arg2);                                                                                                                                                
	}
      else if(command == "import")
        {
          cout << "importing file " <<  command << " lfs file\n";
	}
      else
	{
	  cout << "Need to enter correct commands!";
	}
    }
  return 0;
}
