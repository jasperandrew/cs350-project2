#include "lab7.h"

// ======================== Blocks ======================== //

Block::Block(){
	type = 0;
	count = 0;
	index = -1;
	
	for(int i = 0; i < 1024; i++) data[i] = 0;
}

Block::~Block(){}

void Block::setType(int t){
	if(t < 0 || t > 3){
		cout << "[Error] Invalid block type.\n";
		return;
	}
	type = t;
}

int Block::getType(){
	return type;
}

bool Block::checkType(int t){
	if(t != type){
		cout << "[Error] Invalid function for block type.\n";
		return false;
	}
	return true;
}

// ---------------- data ---------------- //

void Block::setData(char *d){
	if(!checkType(0)) return;
	for(int i = 0; i < 1024; i++)
		if(d[i]) data[i] = d[i];
}

char* Block::getData(){
	if(!checkType(0)) return NULL;
	return data;
}

// ---------------- inode ---------------- //

void Block::setFilename(string f){
	if(!checkType(1)) return;
	filename = f;
}

string Block::getFilename(){
	if(!checkType(1)) return (string)NULL;
	return filename;
}

void Block::addFileBlock(char n){
	if(!checkType(1)) return;
	block_ptrs[count++] = n;
}

int Block::getFileBlocks(){
	if(!checkType(1)) return -1;
	return count;
}

char* Block::writeInode(){
	if(!checkType(1)) return NULL;
	int iPtr = 0;
	for(int i = 0; i < filename.length(); i++) data[iPtr++] = filename[i];
	iPtr++;
	for(int i = 0; i < count; i++) data[iPtr++] = block_ptrs[i];
	return data;
}

// ---------------- imap ---------------- //

void Block::setInodeNum(char oldNum, char newNum){
	if(!checkType(2)) return;
	for(int i = 0; i < 1024; i++){
		if(data[i] == oldNum){
			data[i] = newNum;
			if(DBG) cout << "Updated imap.\n";
			return;
		}
	}
	if(DBG) cout << "inode number not found.\n";
}
//include function getImap(Block *)
//which copies the imap at block into a new imap 
//which can then be modified

bool Block::addInodeNum(char n){
	if(!checkType(2)) return -1;
	data[count++] = n;
}

int Block::getInodeNum(int idx){
	if(!checkType(2)) return -1;
	return data[idx];
}

char* Block::writeImap(){
	if(!checkType(2)) return NULL;	
	return data;
}

//add global variable? to store the 8 segInfo blocks
//
//Store nums in array until inode reached, then loop through array with below function
bool Block::addSegEntry(char n){
    if(!checkType(3)) return -1;
    data[count++] = n;
}

//Actual write needs to be called for 8 nodes
char * Block::writeSegmentNode()
{
    if(!checkType(3)) return NULL;
    return data;
}

// ======================== WriteBuffer ======================== //

WriteBuffer::WriteBuffer(){
	numBlocks = 0;
}

WriteBuffer::~WriteBuffer(){}

void WriteBuffer::addBlock(Block b){
	buf[numBlocks++] = b;
	if(numBlocks == 1016) writeToDisk();
}

void WriteBuffer::writeToDisk(){
	// Segment summary blocks
	int iPtr = 0;
	for(int i = 0; i < numBlocks; i++){
		
	}
	numBlocks = 0;
	if(DBG) cout << "Write buffer written to DISK.\n";			
}

int WriteBuffer::getNumBlocks(){
	return numBlocks;
}

Block WriteBuffer::getBlock(int idx){
	return buf[idx];
}
