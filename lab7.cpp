#include "lab7.h"

int main(int argc, char **argv)
{
  initDrive();
  initFileMap();
  checkPointInit();
  //argument parsing
  string command;
  vector<string> args;
  cout<< "Hi There! Please Enter a Command or press <Enter> to quit:  ";
  while(getline(cin, command) && !command.empty())
    {
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
      list();
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
	  cout << "[Error]  You need to enter the following correct commands with correct parameters! \n";
	  cout << "・import <filename> <lfs_filename>\n";
	  cout << "・remove <lfs_filename>\n";
	  cout << "・list\n";
	  cout << "・exit\n";
	}
      cout<< "Hi There! Please Enter a Command or press <Enter> to quit:  ";
    }

  return 0;
}
