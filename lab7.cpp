#include "lab7.h"


vector< char*> blockBuffer;
unsigned char segInfo[] = new unsigned char[4096];
unsigned char iMap[];
struct segmentInfo{
    unsigned char block;
    unsigned char offset;
};

int import(char * filename, char * lfs_filename)
{
    ifstream inputFile(filename, ifstream::in);
    if(inputFile)
    {
        unsigned char * kbBlock = new unsigned char [1024];
        while(inputFile.get(kbBlock,1025,EOF) && blockBuffer.size()< 1016)
        {
            blockBuffer.push_back(kbBlock);
            //As you add blocks to buffe, add block info to sementInfo array/vector
        }
        if(blockBuffer.size() > 1015)
        {
            //output the buffer to the next segment
        }
        if(!inputFile.get(kbBlock,1025,EOF))
        {
            //write Inode, update imap,yadda yadda   
        }
        else
        {
            //save kbBlock to write int next segment
        }
        for(int i=0; i <blockBuffer.size(); i++)
        {
            cout <<blockBuffer[i] <<endl;
        }
    }    
}

int initDrive()
{
	string path = "DRIVE";
	mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	path += "/SEGMENT";
	for(int i = 0; i < 32; i++){
		FILE *fp = fopen((path + to_string(i+1)).c_str(), "w");
		ftruncate(fileno(fp), 1024*1024);
		fclose(fp);
	}
	return 0;
}


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
    }
  return 0;
}
