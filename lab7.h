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
    ifstream inputFile(filepath, ifstream::in);
    if(inputFile)
    {
        char kbBlock[1024];
        Block iNodeBlock(1);
        iNodeBlock.setFilename(lfs_filename);
        inputFile.seekg (0, ios::end);
        int length = inputFile.tellg();
        inputFile.seekg (0);
        if((1024-wbuffer.getNumBlocks()) <( length+2)) wbuffer.writeToDisk();
        while(1)
        {
            if(inputFile.get(kbBlock,1024,EOF))
            {
                Block tmpBlock(0);
                tmpBlock.setData(kbBlock);
                wbuffer.addBlock(tmpBlock);
                iNodeBlock.addPtr(1024 * wbuffer.getSegCtr() + wbuffer.getNumBlocks());
                //As you add blocks to buffer, add block info to segmentInfo array/vector add info to inode

            }
            else
            {
                wbuffer.addBlock(iNodeBlock);
                iMapList[wbuffer.getInodeCounter(1)] = wbuffer.getNumBlocks();
                Block tmp = wbuffer.createMapBlock();

                wbuffer.addBlock(tmp);
                if(tmp.dataFull()) Checkpoint_Region.imaps[Checkpoint_Region_counter++] = wbuffer.getNumBlocks();
               //write to Checkpoint Region 
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



int initFileMap()
{
    string path = "DRIVE/fileMap";
    ifstream f(path.c_str());

    if(f.good())
    {
        if(DBG) cout << "File Map exist\n";
    }
    FILE *fp = fopen(path.c_str(), "w");
    fclose(fp);
    ofstream oFileMap("DRIVE/fileMap", ios::out | ios::binary);
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

	// Create checkpoint region file
	if(DBG) cout << "Creating checkpoint region file.\n";
	FILE *cr = fopen((path + string("/CHECKPOINT_REGION")).c_str(), "w");
	ftruncate(fileno(cr), 192);
	fclose(cr);

	// Create file map
	if(DBG) cout << "Creating file map file.\n";
	ofstream fm(path + string("/FILE_MAP"), ios::out);
	//fm.seekp();
	//FILE *fm = fopen(.c_str(), "w");
	//ftruncate(fileno(fp), 192);
	//fclose(fm);
	fm.close();
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
