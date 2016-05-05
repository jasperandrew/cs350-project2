#include "lab7.h"

int main(int argc, char **argv)
{
  initDrive();
  initFileMap();
  checkPointInit();
  //argument parsing
  string command;
  vector<string> args;
  while(getline(cin, command) && !command.empty())
    {
      args.clear();     
      boost::algorithm::split(args, command, boost::algorithm::is_any_of(" "));
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
	  wbuffer.writeToDisk();
	  writeCheckpoint();
	  exit(0);
	}
      else
	{
	  cout << "Need to enter correct commands with correct parameters!\n";
	}
    }

  ifstream inFileMap("DRIVE/fileMap");

  if (inFileMap)
  {
      pair<int, string> tmp;
      int iNodenum;
      string line, tmpFileName;
      if(DBG) cout<<"Reading Filename map\n";
      while(getline(inFileMap,line))
      {
          stringstream s(line);
          s >> iNodenum >> tmpFileName;
          tmp.first = iNodenum;
          tmp.second = tmpFileName;
          fileMap.push_back(tmp);
      }


  }
  else
  {
      //write file map on exit
  }
  return 0;
}
