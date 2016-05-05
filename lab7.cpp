#include "lab7.h"

int main(int argc, char **argv)
{
  initDrive();
  initFileMap();
  checkPointInit();
  //argument parsing
  string command;
  vector<string> args;
  while(1)
    { 
    getline(cin, command);
      args.clear();     
      boost::algorithm::split(args, command, boost::algorithm::is_any_of(" "));
      if(args[0] == "import" && args.size() == 3) 
	{
	  cout << "importing file: " <<  args[1] <<" \nwith lfs filename:" << args[2]<< "\n";
	  import(args[1], args[2]);                                                 
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
	  wbuffer.writeToDisk();
	  writeCheckpoint();
      writeFileMap();
	  exit(0);
	}
      else
	{
	  cout << "Need to enter correct commands with correct parameters!\n";
	}
    }

  return 0;
}
