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
		Block(int t);
		~Block();
		int getType();
		char* writeBlock();
		void setData(char *d);
		char* getData();
		void setFilename(string f);
		string getFilename();
		void addPtr(char n);
		int getNumPtrs();
		bool addInodeNum(char n);
		void setInodeNum(char oldNum, char newNum);
		int getInodeNum(int idx);
		void addSegEntry(char n, char m);
		
	private:
		bool checkType(int t);
		int type;
		// data
		char data[1024];
		// inode
		string filename;
		int count;
		char block_ptrs[128];
};

class WriteBuffer {
	public:
		WriteBuffer();
		~WriteBuffer();
		void addBlock(Block b);
		void writeToDisk();
		int getNumBlocks();
		Block getBlock(int idx);
		int getSegCtr();
	private:
		Block buf[1016];
		int num_blocks, seg_counter;
};

class CheckpointRegion {
	
};

// ============================GLOBAL VARIABLES============================== //

WriteBuffer wbuffer;


// ============================ FUNCTIONS ============================ //
int import(string filepath, string lfs_filename)
{
    ifstream inputFile(filepath, ifstream::in);
    if(inputFile)
    {
        char * kbBlock = new char [1024];
        Block *iNodeBlock = new Block(1);
        iNodeBlock->setFilename(lfs_filename);
        while(1)
        {
            if(wbuffer.getNumBlocks() >= 1024)
            {
                //segment info is already written
                wbuffer.writeToDisk();
            }
            if(inputFile.get(kbBlock,1024,EOF))
            {
                Block *tmpBlock = new Block(0);
                tmpBlock->setData(kbBlock);
                wbuffer.addBlock(*tmpBlock);
                iNodeBlock->addPtr(wbuffer.getNumBlocks());
                //As you add blocks to buffer, add block info to segmentInfo array/vector add info to inode

            }
            else
            {
                if(wbuffer.getNumBlocks() < 1024)
                {
                    wbuffer.addBlock(*iNodeBlock);

                }
                else
                {
                }
                //check that buffer has space, write new imap, write to checkpoint variable
                //Set Inode index as int returned by std::hash of lfs_filename
                //when looking for inode, hash filename and find it in imap
                break;
            }
        }
        if(DBG)
        {
        }
    }    
}

/*
void remove(string filename)
{
  //find file inode pointer in imap and remove it
}

void list()
{
  //find and list all files with their sizes 
}
*/

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
