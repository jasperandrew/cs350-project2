#include "lab7.h"

// ======================== Blocks ======================== //

Block::Block(){
	type = 0;
	index = -1;
}

Block::~Block(){}

void Block::setType(int t){
	if(t < 0 || t > 2){
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
		else data[i] = 0;
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

void Block::addFileBlock(){}

int Block::getFileBlocks(){
	if(!checkType(1)) return -1;
	return file_blocks;
}

// ---------------- imap ---------------- //

void Block::setInodeNum(int idx, int n){
	if(!checkType(2)) return;
	inode_ptrs[idx] = n;
}

int Block::getInodeNum(int idx){
	if(!checkType(2)) return -1;
	return inode_ptrs[idx];
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
	
	numBlocks = 0;
	if(DBG) cout << "Write buffer written to DISK.\n";			
}

int WriteBuffer::getNumBlocks(){
	return numBlocks;
}

Block WriteBuffer::getBlock(int idx){
	return buf[idx];
}