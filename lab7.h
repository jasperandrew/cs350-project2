#ifndef LAB7_H_
#define LAB7_H_

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace boost::algorithm;

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
		void addFileBlock();
		int getFileBlocks();
		
	private:
		bool checkType(int t);
		int type;
		int index;
		// data
		char data[1024];
		// inode
		string filename;
		int file_blocks, block_ptrs[128];
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
		Block buf[1024];
		int numBlocks;
};

class CheckpointRegion {
	
};


// ============================ FUNCTIONS ============================ //
int import(char * filename, char * lfs_filename)
{
    ifstream inputFile(filename, ifstream::in);
    if(inputFile)
    {
        char * kbBlock = new char [1024];
        while(1)
        {
            if(newBuf.getNumBlocks() > 1015)
            {
                //segment info is already written
                newBuf.writeToDisk();
            }
            if(inputFile.get(kbBlock,1025,EOF))
            {
                Block tmpBlock();
                tmpBlock.setType(1);
                tmpBlock.setData(kbBlock);
                newBuf.addBlock(kbBlock);
                //As you add blocks to buffer, add block info to segmentInfo array/vector

            }
            else
            {
                Block iNodeBlock();
                iNodeBlock.setType(0);
                string iNodeFileName(lfs_filename);
                iNodeBlock.setFilename(iNodeFileName);
                //check that buffer has space, write new imap, write to checkpoint variable
                //save kbBlock to write int next segment
                break;
            }
        }
        if(DBG)
        {
        }
    }    
}

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
