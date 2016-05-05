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
#include <sstream>

using namespace std;

#define DBG 1
#define BLOCK_SZ 1024
#define SEG_SZ 1024*BLOCK_SZ

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
		bool addImapNum(char n);
        bool dataFull();
		
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
        int getInodeCounter(int increment);
		Block getBlock(int idx);
        Block createMapBlock();
		int getSegCtr();
	private:
		Block buf[1016];
		int num_blocks, seg_counter, inode_counter;
};


// ============================GLOBAL VARIABLES============================== //

WriteBuffer wbuffer;
vector<pair<int, string> > fileMap;

struct Checkpoint_Region
{
  unsigned int imaps[40];
  char liveBits[32];
}Checkpoint_Region;

int iMapList[40960];
int Checkpoint_Region_counter = 0;



// ============================ FUNCTIONS ============================ //
int import(string filepath, string lfs_filename)
{
	ifstream input_file(filepath, ios::in | ios::ate | ios::binary);
	if(input_file){
		int file_len = input_file.tellg();

		Block inode_block(1);
		inode_block.setFilename(lfs_filename);
		//if((1024-wbuffer.getNumBlocks()) <( length+2)) wbuffer.writeToDisk();
		for(int i = 0; i < file_len; i += 1024){
			input_file.seekg(i);
			char tmp_data[1024] = {0};
			input_file.read(tmp_data, 1024);
			
			Block data_block(0);
			data_block.setData(tmp_data);
			
			wbuffer.addBlock(data_block);
			int block_number = 1024 * wbuffer.getSegCtr() + wbuffer.getNumBlocks();
			inode_block.addPtr(block_number);
		}
		
		wbuffer.addBlock(inode_block);
        iMapList[wbuffer.getInodeCounter(1)] = wbuffer.getNumBlocks();
        pair<int,string> tmpPair(wbuffer.getNumBlocks(),inode_block.getFilename());
        fileMap.push_back(tmpPair);
        Block tmp = wbuffer.createMapBlock();
        wbuffer.addBlock(tmp);
        if(tmp.dataFull()) Checkpoint_Region.imaps[Checkpoint_Region_counter++] = wbuffer.getNumBlocks();
	}
	return 0;
}

void checkPointInit()
{
  for(unsigned int imap: Checkpoint_Region.imaps)
    {
      imap = 0;
    }
  for(char liveBit: Checkpoint_Region.liveBits)
    {
      liveBit = 0;
    }
  return;
}

void remove(string filename)
{
    for(int i = 0; i < fileMap.size(); i++)
    {
        if(fileMap[i].second == filename)
        {
            fileMap.erase(fileMap.begin()+i);
        }
    }

}

void list()
{
  //find and list all files with their sizes 
}

void writeCheckpoint()
{ 
  ofstream f;
  f.open("DRIVE/CHECKPOINT_REGION", std::ofstream::out | std::ofstream::trunc);
  for(int i = 0; i < 40; i++)
    {
      f << Checkpoint_Region.imaps[i] << "\n";
    }
 
  for(int k = 0; k < 32; k++)
    {
      f << (int)Checkpoint_Region.liveBits[k] << "\n";
      }
  f.close();
  return;
}

int initFileMap()
{
    string path = "DRIVE/FILE_MAP";
    ifstream f(path.c_str());

    if(f.good())
    {
        if(DBG) cout << "File map exists\n";
    }
    FILE *fp = fopen(path.c_str(), "w");
    fclose(fp);
    ofstream oFileMap("DRIVE/FILE_MAP", ios::out | ios::binary);
    for(int i = 0; i < fileMap.size(); i++)
    {
        oFileMap << fileMap[i].first << "\t" << fileMap[i].second << "\n";
    } 
    oFileMap.close();
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

	// Create file map
	if(DBG) cout << "Creating file map file.\n";
	ofstream filemap(path + string("/FILE_MAP"), ios::out);
	filemap.close();

	// Create checkpoint region file
	if(DBG) cout << "Creating checkpoint region file.\n";
	ofstream checkpoint(path + string("/CHECKPOINT_REGION"), ios::out);
	checkpoint.seekp(192-1);
	checkpoint.write("", 1);
	checkpoint.close();

	// Create segment files
	path += "/SEGMENT";
	for(int i = 0; i < 32; i++){
		if(DBG) cout << "Creating segment file " << i + 1 << ".\n";
		ofstream segment(path + to_string(i+1), ios::out);
		segment.seekp(1024*1024-1);
		segment.write("", 1);
		segment.close();
	}
	
	return 0;
}

#include "classes.cpp"

#endif
