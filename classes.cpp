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

char* Block::writeBlock(){
	if(type == 0 || type == 2) return data;
	if(type == 1){
		int iPtr = 0;
		for(int i = 0; i < filename.length(); i++) data[iPtr++] = filename[i];
		iPtr++;
		for(int i = 0; i < count; i++) data[iPtr++] = block_ptrs[i];
		return data;
	}
	return NULL;
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

bool Block::addInodeNum(char n){
	if(!checkType(2)) return -1;
	data[count++] = n;
}

int Block::getInodeNum(int idx){
	if(!checkType(2)) return -1;
	return data[idx];
}

// ---------------- segment summary ---------------- //



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
	ofstream segment("DRIVE/SEGMENT1", ios::out | ios::binary);
	for(int i = 0; i < numBlocks; i++){
		segment.seekp(1024*i);
		segment << buf[i].writeBlock();
	}
	segment.close();
	numBlocks = 0;
	if(DBG) cout << "Write buffer written to DISK.\n";			
}

int WriteBuffer::getNumBlocks(){
	return numBlocks;
}

Block WriteBuffer::getBlock(int idx){
	return buf[idx];
}