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
		
	}
}

void WriteBuffer::addBlock(Block *b){
	buf[num_blocks++] = b;
	if(num_blocks == 1024) writeToDisk();
}

void WriteBuffer::writeToDisk(){
	// Segment summary blocks
	int nextSegment = 0;
	for(; nextSegment < 32; nextSegment++){
        //cout << "Checkpoint_region.liveBits: " << nextSegment << " is " << (char)Checkpoint_Region.liveBits[nextSegment] << "\n"; 
		if(Checkpoint_Region.liveBits[nextSegment] == 0) break;
	}
	
	if(DBG) cout << "Writing to SEGMENT" + to_string(nextSegment+1) << endl;

	for(int i = 0; i < num_blocks; i++){
		buf[i]->writeToSegment();
	}
	for(int i = 0; i < 12*BLOCK_SZ; i++){
		cout << mem_segment[i];
	}
	FILE *fp;
	size_t nw;
	if ((fp = fopen(("DRIVE/SEGMENT" + to_string(nextSegment+1)).c_str(), "r+")) == NULL) {
		perror("Trouble opening the file in writeSegmentFile"); 
		exit(1);
	}
	nw = fwrite(mem_segment, sizeof(char), SEG_SZ, fp);  
	if (nw != SEG_SZ) {
		perror("Trouble in writeSegmentFile"); 
		fprintf(stderr,"Wrote %d bytes (expected %d).\n", 
			 (int) nw, (int) SEG_SZ); 
		exit(1);
	}
	fclose(fp);
	
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
