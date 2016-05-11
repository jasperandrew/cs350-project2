#ifndef LAB7_H_
#define LAB7_H_

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <limits.h>

#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <errno.h>

using namespace std;

#define DBG 1
#define BLOCK_SZ 1024
#define SEG_SZ 1024*BLOCK_SZ
#define MAX_FILE_BLOCKS 128

// ============================ STRUCTS ============================ //

typedef unsigned int block_num;

typedef struct inode_struct {
    char filename[256];
    int filesize;
    block_num datablocks[MAX_FILE_BLOCKS];
} inode;

typedef struct segsum_entry_struct {
    int one;
    int two;
} segsum_entry;

// ============================ GLOBAL VARIABLES ============================== //
/*
   struct sysdata {
   int curr_segment;
   } g_sysdata;
   */
char mem_segment[SEG_SZ];
int mem_segment_idx;
int current_segment;

vector<pair<int, string>> g_filemap;

struct Checkpoint_Region{
    unsigned int imaps[40];
    char liveBits[32];
} Checkpoint_Region;
int Checkpoint_Region_counter = 0;

struct global_imap_s {
    int list[10240];
    int idx;
} g_imap;

// ============================ CLASSES ============================ //

class Block {
    public:
        Block(int t){
            type = t;
            if(type == 0)
                for(int i = 0; i < BLOCK_SZ; i++) data[i] = 0;
            else if(type == 2)
                for(int i = 0; i < BLOCK_SZ/4; i++) block_data[i] = 0;
            else if(type == 3)
                for(int i = 0; i < BLOCK_SZ/4; i++) block_data[i] = 0;

            num_blocks = 0;
        }

        int getType(){return type;}

        // data
        void setData(char *d){
            for(int i = 0; i < BLOCK_SZ; i++){
                if(d[i]) data[i] = d[i];
                else break;
            }
        }
        void writeToSegment(){
            if(type == 0)
                memcpy(mem_segment+(mem_segment_idx++ * BLOCK_SZ), data, BLOCK_SZ);
            else if(type == 1)
                memcpy(mem_segment+(mem_segment_idx++ * BLOCK_SZ), &inode_data, sizeof(inode));
            else if(type == 2 || type == 3)
                memcpy(mem_segment+(mem_segment_idx++ * BLOCK_SZ), block_data, BLOCK_SZ);
        }

        // inode
        void setSize(int s){ inode_data.filesize = s; }
        int getSize(){ return inode_data.filesize; }
        void setFilename(string f){
            const char *fc = f.c_str();
            memcpy(inode_data.filename, fc, f.length()*sizeof(char));
        }
        char* getFilename(){ return inode_data.filename; }
        void addBlockNum(block_num n){ inode_data.datablocks[num_blocks++] = n; }
        void editBlockNum(block_num old_n, block_num new_n){
            for(int i = 0; i < num_blocks; i++)
                if(inode_data.datablocks[i] == old_n) inode_data.datablocks[i] = new_n;
        }
        int getNumBlocks(){ return num_blocks; }

        // imap

        void addInodeData(int start_index);
        void addToCheckpointRegion();
        block_num * getData()
        {
            return block_data;
        }
        void addInodeBlock(block_num n){
            block_data[num_blocks++] = n; }
        void editInodeBlock(block_num old_n, block_num new_n){
            for(int i = 0; i < num_blocks; i++)
                if(block_data[i] == old_n) block_data[i] = new_n;
        }
        block_num getInodeBlock(int inode_num){ return block_data[inode_num]; }
        bool isFull(){ return block_data[BLOCK_SZ - 1]; }

        // segment summary
        void addBlockNumType(block_num n, int type){block_data[num_blocks++] = n;}
	Block* getInode(int inode_num);

    private:
        int type;
        char data[BLOCK_SZ];
        inode inode_data;
        block_num block_data[BLOCK_SZ/4];
        int num_blocks;
};

class WriteBuffer {
    public:
        WriteBuffer();
        ~WriteBuffer();
        void addBlock(Block *b);
        void writeToDisk();
        int getNumBlocks();
        int getInodeCounter(int increment);
        Block* getBlock(int idx);
        //Block* createImapBlock();
    private:
        Block *buf[BLOCK_SZ];
        int num_blocks, inode_counter;
};

WriteBuffer wbuffer;

// ============================ FUNCTIONS ============================ //

void transferImap(block_num * imap_data)
{
    for(int i = 0; i < 256; i++)
    {
        if(imap_data[i] == 0) return;
        g_imap.list[g_imap.idx++] = imap_data[i];
       // cout << "imap_data: "<< i << " " << g_imap.list[g_imap.idx-1] << "\n";
    }

}


void readImaps()
{
    for(int i = 0; i < 40; i++)
    {
        if((Checkpoint_Region.imaps[i]) == 0){
            Checkpoint_Region_counter = i;
            break;

        }
        int block_number = Checkpoint_Region.imaps[i];
        int seg_idx = block_number/1024;
        int seg_block = block_number % 1024;

        if(seg_idx == current_segment){
            transferImap(wbuffer.getBlock(seg_block-1)->getData());
        }else{
            string seg_path = "DRIVE/SEGMENT" + to_string(seg_idx+1);
            FILE * fp;
            fp = fopen(seg_path.c_str(),"r");
            block_num tmpB[BLOCK_SZ/4];
            fseek(fp, (BLOCK_SZ * (seg_block-1)), SEEK_SET);
            fread(tmpB, sizeof(block_num), BLOCK_SZ/4, fp);
            fclose(fp);
            transferImap(tmpB);
        } 
    }
}

//----------CheckPoint------------------//                                                                                 


void checkPointInit()
{
    FILE * checkpoint_file;
    current_segment = 0;
    checkpoint_file = fopen("DRIVE/CHECKPOINT_REGION","r");
    if(checkpoint_file != NULL){
        if(DBG) cout << "reading checkpoint \n";
        fread(&Checkpoint_Region,sizeof(Checkpoint_Region), 1, checkpoint_file);
        fclose(checkpoint_file);
        for(int i = 0 ; i < 40; i++)
        {
            if(Checkpoint_Region.imaps[i]==0) break;
            //cout << "Imap location: " << Checkpoint_Region.imaps[i] << "\n";
        }

        for(; current_segment < 32; current_segment++){
            if(Checkpoint_Region.liveBits[current_segment] == 0){ 
                //cout <<"livebit: "  << current_segment << " value: " << Checkpoint_Region.liveBits[current_segment];
                //cout <<"current segment: "<< current_segment<< "\n";
                break;
            }
        }
        readImaps();
    }
    else{
        current_segment = 0;
        for(unsigned int imap: Checkpoint_Region.imaps)
        {
            imap = 0;
        }
        for(char liveBit: Checkpoint_Region.liveBits)
        {
            liveBit = 0;
        }
    }
    return;
}
//---------------------------------------//

void writeImaps(){
    for(int i = 0; i < 10240; i+=(BLOCK_SZ/4))
    {
        if(g_imap.list[i] == 0) break;
        Block * imap_block = new Block(2);
        imap_block->addInodeData(i);
    } 
}


//---------------------------------------//  

void writeCheckpoint()
{
    writeImaps();
    FILE * checkpoint_file;
    Checkpoint_Region.liveBits[current_segment] = 1;
    checkpoint_file =  fopen("DRIVE/CHECKPOINT_REGION", "w");
    if(checkpoint_file != NULL)
    {
        fwrite(&Checkpoint_Region,sizeof(Checkpoint_Region), 1, checkpoint_file);
        fclose(checkpoint_file);
    }
    else perror("File Open Failed: ");
    return;
}

void lookForFreeSpots(string lfs_filename)
{
    bool found = false;
    for(int i = 0; i < 10240; i++)
    {
        if(g_imap.list[i] == UINT_MAX)
        {
            g_filemap.push_back(make_pair(i, lfs_filename));
            g_imap.list[i] = BLOCK_SZ * current_segment + wbuffer.getNumBlocks();
            found = true;
        }
    }
    if(!found) cout << "YER OUTTER SPACE!\n";
    return;
}



// --------------------- import --------------------- //
int import(string filepath, string lfs_filename)
{
    ifstream input_file(filepath, ios::in | ios::ate | ios::binary);
    if(input_file){
        int file_len = input_file.tellg();
        Block *inode_block = new Block(1);
        inode_block->setSize(file_len);
        inode_block->setFilename(lfs_filename);

        for(int i = 0; i < file_len; i += BLOCK_SZ){
            input_file.seekg(i);
            char tmp_data[BLOCK_SZ] = {0};
            input_file.read(tmp_data, BLOCK_SZ);

            Block *data_block = new Block(0);
            data_block->setData(tmp_data);

            wbuffer.addBlock(data_block);
            int block_number = BLOCK_SZ * current_segment + wbuffer.getNumBlocks();
            inode_block->addBlockNum(block_number);
        }

        wbuffer.addBlock(inode_block);
        if(g_imap.idx < 10239)
        {
            g_filemap.push_back(make_pair(g_imap.idx, lfs_filename));
            g_imap.list[g_imap.idx++] = BLOCK_SZ * current_segment + wbuffer.getNumBlocks();
        }
        else 
        {
            lookForFreeSpots(lfs_filename);
        }

        for(int n : g_imap.list) if(n) cout << n << " "; else break;

        //Block * imap_block = new Block(2);
        //wbuffer.addBlock(imap_block);

        //if(imap_block->isFull()) Checkpoint_Region.imaps[Checkpoint_Region_counter++] = BLOCK_SZ * current_segment + wbuffer.getNumBlocks()%BLOCK_SZ; 
        //else writeCheckpoint();
    }
    else
    {
        cout<< "File Does Not Exist!\n";
    }
    return 0;
}



// --------------------- remove --------------------- //
void remove(string filename)
{
    bool removed = false;
    unsigned int remove_block;
    for(int i = 0; i < g_filemap.size(); i++){
        if(g_filemap[i].second == filename){
            remove_block = g_filemap[i].first;
            g_filemap.erase(g_filemap.begin()+i);
            removed = true;
            if(DBG) cout << "removed " << filename << "\n";
        }
    }
    if(!removed)
    {
        cout << "file does not exist\n";
        return;
    }
    g_imap.list[remove_block] = UINT_MAX;
    return;
}
/*
   int getFileSize(int inode_num)
   {
   int block_num = g_imap.list[inode_num];
   int seg_idx = block_num/1024;
   int seg_block = block_num % 1024;

   string seg_path = "DRIVE/SEGMENT" + to_string(seg_idx+1);
   cout << inode_num << " " << seg_path << " " << seg_block << " " << BLOCK_SZ * seg_block << " | ";
   ifstream segment(seg_path, ios::in | ios::binary);
   segment.seekg(BLOCK_SZ * seg_block);
   char tmp[1024] = {0};
   segment.read(tmp, BLOCK_SZ);
   cout << tmp; return 0;
   char c = 0;
   while(c != -1){
   segment.get(c);
   cout << c;
   }
   segment.get(c);
   segment.close();
   return (int)c;
   }
   */




// --------------------- list --------------------- //

int getFileSize(int inode_num)
{
    int block_num = g_imap.list[inode_num];
    int seg_idx = block_num/1024;
    int seg_block = block_num % 1024;
    int size;

    if(seg_idx == current_segment){
        return wbuffer.getBlock(seg_block-1)->getSize();
    }else{
        string seg_path = "DRIVE/SEGMENT" + to_string(seg_idx+1);
        ifstream segment(seg_path, ios::in | ios::binary);
        segment.seekg(BLOCK_SZ * (seg_block-1));

        char tmp[sizeof(inode)];
        segment.read(tmp, sizeof(inode));
        segment.close();

        inode tmp_inode;
        memcpy(&tmp_inode, tmp, sizeof(inode));

        return tmp_inode.filesize;
    }
    return -1;
}



//----------ovrwrite--------------//
void overwrite(string filename, string howmany, string start, string c)
{
  int copyNum =  stoi(howmany);
  int sByte = stoi(start);
  char *chr = &c.at(0);                                                                                                                                                        
  int blockNum = 0;
  int size = 0;
  int index = 0;
  for(int i = 0; i < g_filemap.size(); i++)
    {
      if(g_filemap[i].second == filename)
        {
	  blockNum = g_imap.list[g_filemap[i].first];
	  size = g_filemap[i].first;
	  index = i;
        }
    }
  int seg = blockNum/1024;
  int segBlock = blockNum % 1024;
  Block *inode = wbuffer.getBlock(segBlock-1);  
  while(copyNum + sByte > size)
    {
      Block *incBlock = new Block(0);
      char tmp_data[BLOCK_SZ] = {0};
      incBlock->setData(tmp_data);

      if(seg == current_segment)
	{
	  wbuffer.addBlock(incBlock);
	  int dataBlockNum = BLOCK_SZ * current_segment + wbuffer.getNumBlocks();
	  inode->addBlockNum(dataBlockNum);
	}
      else
	{
	  
	}
      g_filemap[index].first = getFileSize(blockNum);
      size = g_filemap[index].first;
    }
  string path = "DRIVE/SEGMENT";
  ofstream segment(path + to_string(seg), ios::out | ios::binary);
  for(int i = 0; i< copyNum;i++)
    {
      segment.seekp(sByte + i);
      segment.write(chr, 1);
      segment.close();
    }
  
  return;
}


// --------------------- list --------------------- //
void list()
{
    for(int i = 0; i < g_filemap.size();i++)
    {
        cout << g_filemap[i].second << " (";
        cout << getFileSize(g_filemap[i].first) << " bytes)\n";
    }
    return;
}

void initFileMap()
{
    string path = "DRIVE/FILE_MAP";
    ifstream filemap(path);

    if(filemap.good())
    {
        pair<int, string> tmp;
        int iNodenum;
        string line, tmpFileName;
        if(DBG) cout << "Reading Filename map\n";
        while(getline(filemap,line))
        {
            stringstream s(line);
            s >> iNodenum >> tmpFileName;
            tmp.first = iNodenum;
            tmp.second = tmpFileName;
            g_filemap.push_back(tmp);
        }
        if(DBG){ cout << "File map exists\n";}
        //return 1;
    }else{
        ofstream ofilemap("DRIVE/FILE_MAP", ios::out | ios::binary);
        for(int i = 0; i < g_filemap.size(); i++)
        {
            ofilemap << g_filemap[i].first << "\t" << g_filemap[i].second << "\n";
        } 
        ofilemap.close();
    }
    return;
}

void writeFileMap()
{
    ofstream ofilemap("DRIVE/FILE_MAP", ios::out | ios::binary);
    for(int i = 0; i < g_filemap.size(); i++)
    {
        ofilemap << g_filemap[i].first << "\t" << g_filemap[i].second << "\n";
    } 
    ofilemap.close();
    return;
}

int initDrive()
{
    string path = "DRIVE";
    ifstream f(path.c_str());
    if(f.good()){
        if(DBG) cout << "DRIVE exists\nLoading data\n";
        return 1;
    }
    /*    else{
          checkPointInit();
          }*/

    if(DBG) cout << "DRIVE does not exist\nCreating DRIVE directory\n";

    // Create drive directory
    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

    // Create file map
    if(DBG) cout << "Creating file map file\n";
    ofstream filemap(path + string("/FILE_MAP"), ios::out);
    filemap.close();

    // Create checkpoint region file
    if(DBG) cout << "Creating checkpoint region file\n";
    /*ofstream checkpoint(path + string("/CHECKPOINT_REGION"), ios::out );
      checkpoint.seekp(192-1);
      checkpoint.write("", 1);
      checkpoint.close();*/

    // Create segment files
    path += "/SEGMENT";
    for(int i = 0; i < 32; i++){
        if(DBG) cout << "Creating segment file " << i + 1 << "\n";
        ofstream segment(path + to_string(i+1), ios::out | ios::binary);
        segment.seekp(SEG_SZ-1);
        segment.write("", 1);
        segment.close();
    }

    g_imap.idx = 0;

    return 0;
}

#include "classes.cpp"

#endif
