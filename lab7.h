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
		Block(){
			type = 0;
			index = -1;
		}
		//~Block(){}
		void setType(int t){
			if(t < 0 || t > 2){
				cout << "[Error] Invalid block type.\n";
				return;
			}
			type = t;
		}
		int getType(){ return type; }
		// data --------------------------------------
		void setData(char *d){
			if(!checkType(0)) return;
			for(int i = 0; i < 1024; i++)
				if(d[i]) data[i] = d[i];
				else data[i] = 0;
		}
		char* getData(){
			if(!checkType(0)) return NULL;
			return data;
		}
		// inode -------------------------------------
		void setFilename(string f){
			if(!checkType(1)) return;
			filename = f;
		}
		string getFilename(){
			if(!checkType(1)) return (string)NULL;
			return filename;
		}
		void addFileBlock(){}
		int getFileBlocks(){
			if(!checkType(1)) return -1;
			return file_blocks;
		}
		// imap --------------------------------------
	private:
		bool checkType(int t){
			if(t != type){
				cout << "[Error] Invalid function for block type.\n";
				return false;
			}
			return true;
		}
		int type;
		int index;
		// data --------------------------------------
		char data[1024];
		// inode -------------------------------------
		string filename;
		int file_blocks, block_ptrs[128];
		// imap --------------------------------------
};

class WriteBuffer {
	public:
		WriteBuffer(){
			numBlocks = 0;
		}
		//~WriteBuffer(){}
		void addBlock(Block b){
			buf[numBlocks++] = b;
			if(numBlocks == 1024){
				numBlocks = 0;
				if(DBG) cout << "Write buffer written to DISK.\n";
			}
		}
		void writeToDisk(){
			numBlocks = 0;
			if(DBG) cout << "Write buffer written to DISK.\n";			
		}
		int getNumBlocks(){ return numBlocks; }
		Block getBlock(int idx){ return buf[idx]; }
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
        while(inputFile.get(kbBlock,1025,EOF) && newBuf.getNumBlocks()< 1016)
        {
            Block tmpBlock();
            tmpBlock.setType(1);
            tmpBlock.setData(kbBlock);
            newBuf.addBlock(kbBlock);
            //As you add blocks to buffe, add block info to sementInfo array/vector
        }
        if(newBuf.getNumBlocks() > 1015)
        {
            //maybe should just call write
            newBuf.writeToDisk();
        }
        if(!inputFile.get(kbBlock,1025,EOF))
        {
            Block iNodeBlock();
            iNodeBlock.setType(0);
            //check that buffer has space, write new imap
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

#endif
