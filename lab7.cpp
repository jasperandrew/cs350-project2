
#include <iostream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <vector>
#include "lab7.h"

using namespace std;
using namespace boost::algorithm;


vector< char[1024]> blockBuffer;

int lab7::import(char * filename, char * lfs_filename)
{
    
    ifstream inputFile(filename, ifstream::in);
    if(inputFile)
    {
        char * kbBlock = new char [1024];
        while(inputFile.get(kbBlock,1025,EOF) && blockBuffer.size()< 1024)
        {
            blockBuffer.push_back(kbBlock);
        }
        for(int i=0; i <blockBuffer.size(); i++)
        {
            cout <<blockBuffer[i] <<endl;
        }
    }    
}

int main(int argc, char **argv)
{
	string path = "DRIVE";
	mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	path += "/SEGMENT";
	for(int i = 0; i < 32; i++){
		mkdir((path + to_string(i+1)).c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	}
	//argument parsing
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
