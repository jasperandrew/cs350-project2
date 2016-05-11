#ifndef LAB7_H_
#define LAB7_H_

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <errno.h>

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
        bool addInodeNum(unsigned int n);
        void setInodeNum(char oldNum, char newNum);
        int getInodeNum(int idx);
        void addSegEntry(char n, char m);
        bool addImapNum(char n);
        bool dataFull();
        void setSize(int length);

    private:
        bool checkType(int t);
        int type;
        // data
        char data[BLOCK_SZ];
        // inode
        string filename;
        int count;
        int size;
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
        Block createImapBlock();
        int getSegCtr();
    private:
        Block buf[BLOCK_SZ];
        int num_blocks, seg_counter, inode_counter;
};


// ============================ GLOBAL VARIABLES ============================== //

WriteBuffer wbuffer;
vector<pair<int, string>> g_filemap;

struct Checkpoint_Region{
    unsigned int imaps[40];
    char liveBits[32];
} Checkpoint_Region;

struct global_imap_s {
    int list[10240];
    int idx;
} g_imap;

int Checkpoint_Region_counter = 0;


// ============================ FUNCTIONS ============================ //
//


//----------CheckPoint------------------//                                                                                 


void checkPointInit()
{
    FILE * checkpoint_file;
    checkpoint_file = fopen("DRIVE/CHECKPOINT_REGION","r+");
    if(checkpoint_file != NULL){
        if(DBG) cout << "reading checkpoint \n";
        fread(&Checkpoint_Region,sizeof(Checkpoint_Region), 1, checkpoint_file);
        //memcpy(&Checkpoint_Region,checkpoint_file,sizeof(checkpoint_file));
        fclose(checkpoint_file);
    }
    else{
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

// Need to write to disc after every import

void writeCheckpoint()
{
    FILE * checkpoint_file;
    checkpoint_file =  fopen("DRIVE/CHECKPOINT_REGION", "w");
    if(checkpoint_file != NULL)
    {
        fwrite(&Checkpoint_Region,sizeof(Checkpoint_Region), 1, checkpoint_file);
        //memcpy(checkpoint_file,&Checkpoint_Region, sizeof(Checkpoint_Region));
        fclose(checkpoint_file);
    }
    else perror("File Open Failed: ");
    return;
}

// --------------------- import --------------------- //
int import(string filepath, string lfs_filename)
{
    ifstream input_file(filepath, ios::in | ios::ate | ios::binary);
    if(input_file){
        int file_len = input_file.tellg();
        Block inode_block(1);
        inode_block.setSize(file_len);
        inode_block.setFilename(lfs_filename);

        for(int i = 0; i < file_len; i += BLOCK_SZ){
            input_file.seekg(i);
            char tmp_data[BLOCK_SZ] = {0};
            input_file.read(tmp_data, BLOCK_SZ);

            Block data_block(0);
            data_block.setData(tmp_data);

            wbuffer.addBlock(data_block);
            int block_number = BLOCK_SZ * wbuffer.getSegCtr() + wbuffer.getNumBlocks();
            inode_block.addPtr(block_number);
        }

        wbuffer.addBlock(inode_block);

        g_filemap.push_back(make_pair(g_imap.idx, lfs_filename));
        g_imap.list[g_imap.idx++] = BLOCK_SZ * wbuffer.getSegCtr() + wbuffer.getNumBlocks();
        for(int n : g_imap.list) if(n) cout << n << " "; else break;

        Block imap_block = wbuffer.createImapBlock();
        wbuffer.addBlock(imap_block);

        if(imap_block.dataFull()) Checkpoint_Region.imaps[Checkpoint_Region_counter++] = wbuffer.getNumBlocks();
        else writeCheckpoint();
    }
    else
    {
        cout<< "File Does Not Exist!\n";
    }
    return 0;
}



void overwrite(string filename, string howmany, string start, string c)
{
    int copyNum =  stoi(howmany);
    int sByte = stoi(start);
    //char chr = c.charAt(0);
    int blockNum = 0;
    /*if(copyNum + sByte > size)
      {
      increse file size
      }*/
    for(int i = 0; i < g_filemap.size(); i++){
        if(g_filemap[i].second == filename)
        {
            blockNum = g_imap.list[g_filemap[i].first];
        }
    }
    return;
}

//---------------------------------------//



// --------------------- remove --------------------- //
void remove(string filename)
{
    for(int i = 0; i < g_filemap.size(); i++){
        if(g_filemap[i].second == filename){
            g_filemap.erase(g_filemap.begin()+i);
            if(DBG) cout << " happened in remove\n";
        }
        else
        {
            cout << "File Does Not Exist!\n";
        }
    }
    return;
}
/*
   int readInode(int block_num, string filename)
   =======

   int getFileSize(int inode_num)
   >>>>>>> 375ff11010f9fc5ffca734307b1c6b5d0e0824c7
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
   <<<<<<< HEAD
   */



// --------------------- list --------------------- //
void list()
{
    for(int i = 0; i < g_filemap.size();i++)
    {
        cout << g_filemap[i].second<< " | ";
        //cout << getFileSize(g_filemap[i].first) << "\n";
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
        if(DBG) cout<<"Reading Filename map\n";
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
