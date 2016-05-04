#include "lab7.h"

int main(int argc, char **argv)
{
  //argument parsing
  string command;
  vector<string> args;
  while(getline(cin, command) && !command.empty())
    {
      args.clear();     
      split(args, command, is_any_of(" "));
      if(args[0] == "import" && args.size() == 3) 
	{
	  cout << "importing file: " <<  args[1] <<" \nwith lfs filename:" << args[2]<< "\n";
	  //import(args1, args2);                                                 
	}

      else if(args[0] == "remove" && args.size() == 2)
	{
	  cout << "Removing: " <<  args[1] << "\n";
          
	}
      else if(args[0] == "list" && args.size() == 1)
	{
	  cout << "Listing all files...\n";
	}
      else if(args[0] == "exit" && args.size() == 1)
	{
	  cout << "Exiting...\n";
	  exit(0);
	}
      else
	{
	  cout << "Need to enter correct commands with correct parameters!\n";
	}
    }
  return 0;
}
