#include "lab7.h"

// ======================== Blocks ======================== //

Block::Block(){
	type = 0;
	count = 0;
	size = 0;
	
	for(int i = 0; i < 1024; i++) data[i] = 0;
}

Block::Block(int t){
	if(t < 0 || t > 3){
		cout << "[Error] Invalid block type.\n";
		exit(1);
	}
	type = t;
	count = 0;
	size = 0;
	
	for(int i = 0; i < 1024; i++) data[i] = 0;
	if(type == 3)
		for(char c : "segment summary") data[count++] = c;
}

Block::~Block(){}

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
	if(type == 0 || type == 2 || type == 3) return data;
	if(type == 1){
        string size_string;
		int iPtr = 0;
        data[iPtr++] = -1;
		for(int i = 0; i < filename.length(); i++) data[iPtr++] = filename[i];
		data[iPtr++] = -1;
        size_string = to_string(size);
		for(int i = 0; i < size_string.length(); i++) data[iPtr++] = size_string[i];
		data[iPtr++] = -1;
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

void Block::setSize(int length){
    if(!checkType(1)) return;
    size = length;
}

void Block::setFilename(string f){
	if(!checkType(1)) return;
	filename = f;
}

bool Block::dataFull(){
	if(!checkType(2)) return false;
	if (data[1023]) return true;
	return false;
}

string Block::getFilename(){
	if(!checkType(1)) return (string)NULL;
	return filename;
}

void Block::addPtr(char n){
	if(!checkType(1)) return;
	block_ptrs[count++] = n;
	size++;
}

int Block::getNumPtrs(){
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




//include function getImap(Block *)
//which copies the imap at block into a new imap 
//which can then be modified


bool Block::addInodeNum(unsigned int n){
	if(!checkType(2)) return false;
	data[n] = wbuffer.getNumBlocks();
    int lowestIndex = n/1024;
    lowestIndex = lowestIndex*1024;
    for(int x = lowestIndex; x < n; x++)
    {
        data[x-lowestIndex] = iMapList[x];
    }
	return true;
}

int Block::getInodeNum(int idx){
	if(!checkType(2)) return -1;
	return data[idx];
}

// ---------------- Checkpoint ---------------------- //




// ---------------- segment summary ---------------- //



//add global variable? to store the 8 segInfo blocks
//
//Store nums in array until inode reached, then loop through array with below function
void Block::addSegEntry(char n, char m){
    if(!checkType(3)) return;
    data[count++] = n;
		data[count++] = m;
}

// ======================== WriteBuffer ======================== //

WriteBuffer::WriteBuffer(){
	num_blocks = 8;
	seg_counter = 0;
    inode_counter = 0;
	Block s(3);
	for(int i = 0; i < 8; i++) buf[i] = s;
}

WriteBuffer::~WriteBuffer(){}

void WriteBuffer::addBlock(Block b){
	buf[num_blocks++] = b;
	if(num_blocks == 1024) writeToDisk();
}

void WriteBuffer::writeToDisk(){
	// Segment summary blocks
	int nextSegment = 0;
	for(; nextSegment < 32; nextSegment++){
		if(Checkpoint_Region.liveBits[nextSegment] == 0) break;
	}
	
	if(DBG) cout << "Writing to SEGMENT" + to_string(nextSegment+1) << endl;

	ofstream segment("DRIVE/SEGMENT" + to_string(nextSegment+1), ios::out | ios::binary);
	for(int i = 0; i < num_blocks; i++){
		segment.seekp(1024*i);
		char tmp_data[1024] = {0};
		for(int j = 0; j < BLOCK_SZ; j++)
			tmp_data[j] = buf[i].writeBlock()[j];
		segment << tmp_data;
	}
	segment.close();
	Checkpoint_Region.liveBits[nextSegment] = 1;
	num_blocks = 8;
	seg_counter++;
}

int WriteBuffer::getInodeCounter(int increment){
    int ret = inode_counter;
    inode_counter += increment;
    return ret;
}
int WriteBuffer::getNumBlocks(){
	return num_blocks;
}

Block WriteBuffer::createMapBlock()
{
    Block mapBlock(2);
    int currentIndex  = getInodeCounter(0);
    mapBlock.addInodeNum(currentIndex);
    //^ adds the current (correct?) inode list to this block
    return mapBlock;
}

Block WriteBuffer::getBlock(int idx){
	return buf[idx];
}

int WriteBuffer::getSegCtr(){
	return seg_counter;
}
