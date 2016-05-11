#include "lab7.h"

vector<string> &split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim)){
        elems.push_back(item);
    }
    return elems;
}

int main(int argc, char **argv)
{
	cout << "Greetings, [INSERT_SUBJECT_NAME_HERE]!\n"
	        "Welcome to the Log-Indexed File Emulator, or L.I.F.E.\n"
	        "Wait just a moment while we set up a few things...\n\n";
	
	initDrive();
	initFileMap();
	checkPointInit();

	cout << "\nDone! Now it's time for you to grab L.I.F.E. by the horns!\n"
	        "Enter a command, or enter 'help' for usage information.\n"
	        "\n{epic prompt} ";

  string command;
  vector<string> args;
  while(getline(cin, command))
	{
		args.clear();
		split(command, ' ', args);
		if(!command.empty()){
			if((args[0] == "import" || args[0] == "i") && args.size() == 3) 
			{
				cout << "Importing file '" << args[2] << "' from " << args[1]<< "\n";
				import(args[1], args[2]);                                                 
			}
			else if((args[0] == "remove" || args[0] == "r") && args.size() == 2)
			{
				cout << "Removing: " << args[1] << "\n";
				remove(args[1]);
			}
			else if((args[0] == "list" || args[0] == "l") && args.size() == 1)
			{
				cout << "Listing all files...\n";
				list();
			}
			else if((args[0] == "read" || args[0] == "l") && args.size() == 1)
			{
				FILE *segment;
				segment = fopen("DRIVE/SEGMENT1", "r");
				char tmp[SEG_SZ];
				size_t poop = fread(tmp, sizeof(char), SEG_SZ, segment);
				cout << poop;
				for(int i = 0; i < SEG_SZ; i++) cout << tmp[i];
				fclose(segment);
			}
			else if((args[0] == "exit" || args[0] == "q") && args.size() == 1)
			{
				cout << "Getting L.I.F.E. together...\n\n";
				wbuffer.writeToDisk();
				writeCheckpoint();
				writeFileMap();
				cout << "\nQuitting L.I.F.E.\n\u0CA0_\u0CA0\n";
				exit(0);
			}
			else if((args[0] == "help" || args[0] == "h"))
			{
				if(args.size() == 1){
					cout << "Thanks for calling L.I.F.E. help.\nHeaven knows we all need it from time to time.\nValid L.I.F.E. commands:\n"
									"\timport <filename> <lfs_filename>\n"
									"\tremove <lfs_filename>\n"
									"\tlist\n"
									"\texit\n"
									"For more details on a command, enter 'help <command>'.\n";
				}else if(args.size() == 2){
					if(args[1] == "import"){
						cout << "Usage: import <filepath> <lfs_filename>\n"
						        "Info:  Imports a file from a filepath to the DRIVE, using the given filename.\n";
					}
					else if(args[1] == "remove"){
						cout << "Usage: remove <lfs_filename>\n"
						        "Info:  Removes a file from the DRIVE.\n";
					}
					else if(args[1] == "list"){
						cout << "Usage: list\n"
						        "Info:  Lists the names and sizes of all files currently in the DRIVE.\n";
					}
					else if(args[1] == "exit"){
						cout << "Usage: exit\n"
						        "Info:  C'mon bro. Take a good guess.\n";
					}
					else if(args[1] == "help"){
						cout << "Usage: help <command>\n"
						        "Info:  Prints usage info for all commands, and optionally prints specific info for one command.\n";
					}else if(args[1] == "me"){
						cout << "You're not alone; many of us have had suicidal thoughts at some point in our lives. "
						        "Feeling suicidal is not a character defect, and it doesn't mean that you are crazy, or weak, or flawed. "
										"It only means that you have more pain than you can cope with right now. "
										"This pain seems overwhelming and permanent at the moment. "
										"But with time and support, you can overcome your problems and the pain and suicidal feelings will pass.\n"
										"If you’re feeling suicidal right now, please call for help!\n\n"
										"Call 1-800-273-TALK in the U.S.\n"
										"Or visit IASP(http://www.iasp.info/resources/Crisis_Centres) to find a helpline in your country. "
										"Alternatively, talk to someone you trust and let them know how bad things are.\n";
					}else{
						cout << "'" << args[1] << "' is not a command... Take your funny business elsewhere. This is a serious, real-world program.\n";
					}
				}else{
					cout << "Hey, don't get too crazy, now.\n";
				}
			}
			else
			{
				cout << "Invalid command and/or parameters. Enter 'help' for...y'know...help.\n";
			}
		}
		cout << "\n{epic prompt} ";
	}
	return 0;
}
