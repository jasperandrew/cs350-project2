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
#define MAX_FILE_SZ MAX_FILE_BLOCKS*BLOCK_SZ
#define FILE_LIMIT 10240

// ============================ STRUCTS ============================ //

typedef unsigned int block_num;

typedef struct seg_list{
    int size;
    block_num inodes[BLOCK_SZ];
} inode_list;

typedef struct segment_struct {
    unsigned int inode_number;
    unsigned int block_number;
} segment_entry;

typedef struct inode_struct {
    char filename[256];
    int filesize;
    block_num datablocks[MAX_FILE_BLOCKS];
} inode;

typedef struct filemap_entry_struct {
    int num;
    string name;
} filemap_entry;

// ============================ GLOBAL VARIABLES ============================== //

char mem_segment[SEG_SZ];
int mem_segment_idx;
int current_segment;

char block_buffer[BLOCK_SZ];

filemap_entry g_filemap[FILE_LIMIT];
int g_filemap_ctr = 0;

struct Checkpoint_Region{
    unsigned int imaps[40];
    char liveBits[32];
} Checkpoint_Region;
int Checkpoint_Region_counter = 0;

struct global_imap_s {
    int list[FILE_LIMIT];
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
            else if(type == 3){
               seg_inodes.size=0; 
               for(int i = 0; i < BLOCK_SZ/8; i++){ 
                   seg_sum_arr[i].inode_number = 0;
                   seg_sum_arr[i].block_number = 0;
               }
            }
            num_blocks = 0;
						//initInode(&inode_data);
        }

        int getType(){ return type;}

        // data
        void setData(char *d){
            for(int i = 0; i < BLOCK_SZ; i++){
                if(d[i]) data[i] = d[i];
                else break;
            }
        }
				char* getData(){ return data; }
        void writeToSegment(){
            if(type == 0)
                memcpy(mem_segment+(mem_segment_idx++ * BLOCK_SZ), data, BLOCK_SZ);
            else if(type == 1)
                memcpy(mem_segment+(mem_segment_idx++ * BLOCK_SZ), &inode_data, sizeof(inode));
            else if(type == 2)
                memcpy(mem_segment+(mem_segment_idx++ * BLOCK_SZ), block_data, BLOCK_SZ);
            else if(type ==3)
                memcpy(mem_segment+(mem_segment_idx++ * BLOCK_SZ), seg_sum_arr, BLOCK_SZ);
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
				inode getInode(){ return inode_data; }

        // imap

        void addInodeData(int start_index);
        void addToCheckpointRegion();
        block_num* getBlockData(){ return block_data; }
        void addInodeBlock(block_num n){
            block_data[num_blocks++] = n; }
        void editInodeBlock(block_num old_n, block_num new_n){
            for(int i = 0; i < num_blocks; i++)
                if(block_data[i] == old_n) block_data[i] = new_n;
        }
        block_num getInodeBlock(int inode_num){ return block_data[inode_num]; }
        bool isFull(){ return block_data[BLOCK_SZ - 1]; }

        // segment summary
        void addBlockNumSum(block_num n, block_num x){
            seg_sum_arr[num_blocks].block_number = n;
            seg_sum_arr[num_blocks++].inode_number = x;
            if(n==x) seg_inodes.inodes[seg_inodes.size++] = x;
        }
        inode_list getSegInodes(){ return seg_inodes;}
        void clearSegSum(){
         for(int i = 0; i < BLOCK_SZ/8; i++){ 
             seg_sum_arr[i].block_number = 0;
             seg_sum_arr[i].inode_number = 0;
         }
        }
        

    private:
        int type;
        char data[BLOCK_SZ];
        inode inode_data;
        block_num block_data[BLOCK_SZ/4];
        int num_blocks;
        segment_entry seg_sum_arr[BLOCK_SZ/8];
        inode_list seg_inodes;
};

class WriteBuffer {
    public:
        WriteBuffer();
        ~WriteBuffer();
        void addBlock(Block *b);
        void writeToDisk();
        int getNumBlocks();
        int getInodeCounter(int increment);
        void addSegInfo(block_num * data_blocks, block_num inode_block);
        Block* getBlock(int idx);
        //Block* createImapBlock();
    private:
        Block *buf[BLOCK_SZ];
        int num_blocks, inode_counter;
        Block *seg_summary[8];
};

WriteBuffer wbuffer;


// ============================ IMAP FUNCTIONS ============================ //

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
            //cout << current_segment<< "\n";
            transferImap(wbuffer.getBlock(seg_block-1)->getBlockData());
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

void writeImaps(){
    for(int i = 0; i < 10240; i+=(BLOCK_SZ/4))
    {
        if(g_imap.list[i] == 0) break;
        Block * imap_block = new Block(2);
        imap_block->addInodeData(i);
    } 
}


// ============================ CHECKPOINT FUNCTIONS ============================ //

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
            g_filemap[g_filemap_ctr].num = i;
            g_filemap[g_filemap_ctr].name = lfs_filename;
						g_filemap_ctr++;
						
            g_imap.list[i] = BLOCK_SZ * current_segment + wbuffer.getNumBlocks();
            found = true;
        }
    }
    if(!found) cout << "YER OUTTER SPACE!\n";
    return;
}


// ============================ FILEMAP FUNCTIONS ============================ //

void readFileMap()
{
	ifstream filemap("DRIVE/FILE_MAP", ios::in);
	if(DBG) cout << "Reading from FILE_MAP\n";
	g_filemap_ctr = 0;
	string line;
	while(getline(filemap,line))
	{
		stringstream s(line);
		s >> g_filemap[g_filemap_ctr].num >> g_filemap[g_filemap_ctr].name;
		g_filemap_ctr++;
	}
	filemap.close();
}

void writeFileMap()
{
	ofstream filemap("DRIVE/FILE_MAP", ios::out);
	if(DBG) cout << "Writing to FILE_MAP\n";
	for(int i = 0; i < g_filemap_ctr; i++){
		filemap << g_filemap[i].num << "\t" << g_filemap[i].name << "\n";
	} 
	filemap.close();
}


// ============================ HELPER FUNCTIONS ============================ //

inode readInode(int inode_num)
{
    int inode_block = g_imap.list[inode_num];
    int seg_idx = inode_block/BLOCK_SZ;
    int seg_block = inode_block % BLOCK_SZ;
		inode tmp_inode;

    if(seg_idx == current_segment){
        tmp_inode = wbuffer.getBlock(seg_block-1)->getInode();
    }else{
        string seg_path = "DRIVE/SEGMENT" + to_string(seg_idx+1);
        ifstream segment(seg_path, ios::in | ios::binary);
        segment.seekg(BLOCK_SZ * (seg_block-1));

        char tmp[sizeof(inode)];
        segment.read(tmp, sizeof(inode));
        segment.close();

        memcpy(&tmp_inode, tmp, sizeof(inode));
		}
    return tmp_inode;
}

void readData(block_num num)
{
		for(int i = 0; i < BLOCK_SZ; i++) block_buffer[i] = 0;
	
    int seg_idx = num/BLOCK_SZ;
    int seg_block = num % BLOCK_SZ;

    if(seg_idx == current_segment){
        memcpy(block_buffer, wbuffer.getBlock(seg_block-1)->getData(), BLOCK_SZ);
    }else{
        string seg_path = "DRIVE/SEGMENT" + to_string(seg_idx+1);
        ifstream segment(seg_path, ios::in | ios::binary);
        segment.seekg(BLOCK_SZ * (seg_block-1));

				segment.read(block_buffer, BLOCK_SZ);
        segment.close();
		}
}

int getInodeNum(string filename){
	for(int i = 0; i < g_filemap_ctr; i++){
		if(g_filemap[i].name == filename) return g_filemap[i].num;
	}
	return -1;
}


// ==================================================================================================== //
// ========================================== MAIN FUNCTIONS ========================================== //
// ==================================================================================================== //

// ============================ IMPORT ============================ //
int import(string filepath, string filename)
{
	cout << "Importing '" << filepath << "' --> " << filename << "\n";
	ifstream input_file(filepath, ios::in | ios::ate | ios::binary);
    block_num blocknums[128] = {0};
	if(input_file){
		int file_len = input_file.tellg();
		Block *inode_block = new Block(1);
		inode_block->setSize(file_len);
		inode_block->setFilename(filename);

		for(int i = 0; i < file_len; i += BLOCK_SZ){
			input_file.seekg(i);
			char tmp_data[BLOCK_SZ] = {0};
			input_file.read(tmp_data, BLOCK_SZ);

			Block *data_block = new Block(0);
			data_block->setData(tmp_data);

			wbuffer.addBlock(data_block);
			int block_number = BLOCK_SZ * current_segment + wbuffer.getNumBlocks();
			inode_block->addBlockNum(block_number);
            blocknums[i/BLOCK_SZ] = block_number;
		}

		wbuffer.addBlock(inode_block);
        wbuffer.addSegInfo(blocknums,wbuffer.getNumBlocks());

		g_filemap[g_filemap_ctr].num = g_imap.idx;
		g_filemap[g_filemap_ctr].name = filename;
		g_filemap_ctr++;

		g_imap.list[g_imap.idx++] = BLOCK_SZ * current_segment + wbuffer.getNumBlocks(); 

		//Block * imap_block = new Block(2);
		//wbuffer.addBlock(imap_block);

		//if(imap_block->isFull()) Checkpoint_Region.imaps[Checkpoint_Region_counter++] = BLOCK_SZ * current_segment + wbuffer.getNumBlocks()%BLOCK_SZ; 
		//else writeCheckpoint();
	}
	else
	{
		cout << "File '" << filename << "' does not exist!\n";
	}
	return 0;
}


// ============================ REMOVE ============================ //
void remove(string filename)
{
	bool removed = false;
	unsigned int remove_block;
	for(int i = 0; i < g_filemap_ctr; i++){
		if(g_filemap[i].name == filename){
			remove_block = g_filemap[i].num;
			g_filemap[i].num = -1;
			removed = true;
			if(DBG) cout << "Removed " << filename << "\n";
		}
	}
	if(!removed)
	{
	cout << "File '" << filename << "' does not exist!\n";
	return;
	}
	g_imap.list[remove_block] = UINT_MAX;
}


// ============================ OVERWRITE ============================ //
void overwrite(string filename, string howmany, string start, string c)
{
  //convert strings args to ints                                                                                                                                                                                                                              
  int copyNum =  stoi(howmany);
  int sByte = stoi(start);
  char *chr = &c.at(0);
  block_num blocknums[128] = {0};
  char bigBuffer[MAX_FILE_SZ];
  inode node;
  //get inode Block Num and file map index                                                                                                                                                                                                                   
  for(int i = 0; i < g_filemap_ctr; i++)
    {
      if(g_filemap[i].name == filename)
        {
	  node = readInode(g_filemap[i].num);
          //blockNum = g_imap.list[g_filemap[i].num];
        }
    }
  /*Block inode*/ 
  //inode node = readInode(blockNum);
  Block *inodeB = new Block(1);
  for(int i = 0; i < 128;i++)
    {
      if(node.datablocks[i])
	{
 	  readData(node.datablocks[i]);
	  memcpy(bigBuffer+(i*BLOCK_SZ), block_buffer, BLOCK_SZ);	  
	}
    }
  
  for(int j = sByte; j < copyNum+sByte;j++)
    {
      bigBuffer[j] = *chr;
    }
   

  for(int i = 0; i < MAX_FILE_SZ; i += BLOCK_SZ){
    
    char tmp_data[BLOCK_SZ] = {0};
    memcpy(tmp_data, bigBuffer+i, BLOCK_SZ); 

    Block *data_block = new Block(0);
    data_block->setData(tmp_data);

    wbuffer.addBlock(data_block);
    int block_number = BLOCK_SZ * current_segment + wbuffer.getNumBlocks();
    inodeB->addBlockNum(block_number);
    blocknums[i/BLOCK_SZ] = block_number;
  }

  wbuffer.addBlock(inodeB);
  wbuffer.addSegInfo(blocknums,wbuffer.getNumBlocks());

  for(int s; s<g_filemap_ctr; s++)
    {
      if (g_filemap[s].name == filename)
        {
          g_filemap[s].name == "";
          g_filemap[s].num == -1;
        }
    }

  g_filemap[g_filemap_ctr].num = g_imap.idx;
  g_filemap[g_filemap_ctr].name = filename;
  g_filemap_ctr++;
  

  g_imap.list[g_imap.idx++] = BLOCK_SZ * current_segment + wbuffer.getNumBlocks();
  return;
}


// ============================ LIST ============================ //
void list()
{
	cout << "\nFile List\n----------------------------\n";
	for(int i = 0; i < g_filemap_ctr;i++){
		cout << g_filemap[i].name << " (";
		cout << readInode(g_filemap[i].num).filesize << " bytes)\n";
	}
}


// ============================ CAT ============================ //
void cat(string filename)
{		
		int inode_num = getInodeNum(filename);
		if(inode_num == -1){
			cout << "File '" << filename << "' does not exist!\n";
			return;
		}
		inode file_inode = readInode(inode_num);
		for(int i = 0; i < MAX_FILE_BLOCKS; i++){
			if(file_inode.datablocks[i]){
				readData(file_inode.datablocks[i]);
				printf("%.*s", BLOCK_SZ, block_buffer);
			}else{
				break;
			}
		}
		cout << endl;
}


// ============================ DISPLAY ============================ //
void display(string filename, int how_many, int start)
{
		int inode_num = getInodeNum(filename);
		if(inode_num == -1){
			cout << "File '" << filename << "' does not exist!\n";
			return;
		}
		
		int start_block = start/BLOCK_SZ;
		int start_offset = start % BLOCK_SZ;
		int bytes_read = 0;
		
		inode file_inode = readInode(inode_num);
		for(int i = start_block; i < MAX_FILE_BLOCKS; i++){
			if(file_inode.datablocks[i]){
				if(bytes_read < how_many){
					readData(file_inode.datablocks[i]);
					if(how_many - bytes_read >= BLOCK_SZ)
						printf("%.*s", BLOCK_SZ, block_buffer);
					else
						printf("%.*s", how_many-bytes_read, block_buffer);
				}else{
					cout << "\n\nComplete! (read " << how_many << " bytes)\n";
					return;
				}
			}else{
				cout << "\n\nReached end of file (read " << bytes_read << " bytes)\n";
				return;
			}
		}
		
		if(bytes_read < how_many)
			cout << "\n\nReached end of file (read " << bytes_read << " bytes)\n";
		else
			cout << "\n\nComplete! (read " << how_many << " bytes)\n";
}


// ============================ INITDRIVE ============================ //
int initDrive()
{
    string path = "DRIVE";
    ifstream f(path.c_str());
    if(f.good()){
        if(DBG) cout << "Loading data from existing DRIVE\n";
        return 1;
    }

    if(DBG) cout << "Creating DRIVE directory\n";

    // Create drive directory
    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

    // Create file map
    if(DBG) cout << "Creating FILE_MAP file\n";
    ofstream filemap(path + string("/FILE_MAP"), ios::out);
    filemap.close();

    // Create checkpoint region file
    if(DBG) cout << "Creating CHECKPOINT_REGION file\n";
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
