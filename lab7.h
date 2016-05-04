#ifndef LAB7_H_
#define LAB7_H_

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#define DBG 1

// ============================ CLASSES ============================ //

class Block {
	public:
		Block();
		~Block();
		void setType(int t);
		int getType();
		void setData(char *d);
		char* getData();
		void setFilename(string f);
		string getFilename();
		void addFileBlock(char n);
		int getFileBlocks();
		bool addInodeNum(char n);
		void setInodeNum(char oldNum, char newNum);
		int getInodeNum(int idx);
		char* writeImap();
		char* writeInode();
		
	private:
		bool checkType(int t);
		int type;
		int index;
		// data
		char data[1024];
		// inode
		string filename;
		int count;
		char block_ptrs[128];
		// imap
};

class WriteBuffer {
	public:
		WriteBuffer();
		~WriteBuffer();
		void addBlock(Block b);
		void writeToDisk();
		int getNumBlocks();
		Block getBlock(int idx);
	private:
		Block buf[1016];
		int numBlocks;
};

class CheckpointRegion {
	
};


// ============================ FUNCTIONS ============================ //
/*
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
}*/

int initDrive()
{
	string path = "DRIVE";
	ifstream f(path.c_str());
  if(f.good()){
		if(DBG) cout << "DRIVE exists.\nLoading data.\n";
		return 1;
	}
	
	if(DBG) cout << "DRIVE does not exist.\nCreating DRIVE directory.\n";
	
	// Create drive directory
	mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

	// Create checkpoint region file
	if(DBG) cout << "Creating checkpoint region file.\n";
	FILE *fp = fopen((path + string("/CHECKPOINT_REGION")).c_str(), "w");
	ftruncate(fileno(fp), 320);
	fclose(fp);

	// Create segment files
	path += "/SEGMENT";
	for(int i = 0; i < 32; i++){
		if(DBG) cout << "Creating segment file " << i + 1 << ".\n";
		FILE *fp = fopen((path + to_string(i+1)).c_str(), "w");
		ftruncate(fileno(fp), 1024*1024);
		fclose(fp);
	}
	
	return 0;
}

#include "classes.cpp"

#endif
