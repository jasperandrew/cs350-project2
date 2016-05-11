#include "lab7.h"

/*/ ---------------- segment summary ---------------- //

void Block::addSegEntry(char n, char m){
    if(!checkType(3)) return;
    data[count++] = n;
		data[count++] = m;
}*/

// ======================== WriteBuffer ======================== //

WriteBuffer::WriteBuffer(){
	num_blocks = 0;
	inode_counter = 0;
	for(int i = 0; i < 8; i++){
		Block *s = new Block(3);
		addBlock(s);
	}
}

WriteBuffer::~WriteBuffer(){
	for(int i = 0; i < BLOCK_SZ; i++){
		if(buf[i]) delete buf[i];
	}
}

void WriteBuffer::addBlock(Block *b){
	buf[num_blocks++] = b;
	if(num_blocks == 1024) writeToDisk();
}

void WriteBuffer::writeToDisk(){
	// Segment summary blocks
	int nextSegment = current_segment;
	/*for(; nextSegment < 32; nextSegment++){
		if(Checkpoint_Region.liveBits[nextSegment] == 0) break;
	}*/
	
	if(DBG) cout << "Writing to SEGMENT" + to_string(nextSegment+1) << endl;

	for(int i = 0; i < num_blocks; i++){
		buf[i]->writeToSegment();
	}
	
	ofstream segment("DRIVE/SEGMENT" + to_string(nextSegment+1), ios::out | ios::binary);
	segment.write(mem_segment, SEG_SZ);
	segment.close();
	
	Checkpoint_Region.liveBits[nextSegment] = 1;
	num_blocks = 8;
	current_segment++;
}

int WriteBuffer::getNumBlocks(){
	return num_blocks;
}

Block* WriteBuffer::createImapBlock()
{
    Block *imap_block = new Block(2);
    //imap_block.addInodeNum(currentIndex); // DO THIS
    return imap_block;
}

Block* WriteBuffer::getBlock(int idx){
	return buf[idx];
}
