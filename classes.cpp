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
        seg_summary[i] = new Block(3);
        addBlock(seg_summary[i]);
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
    for(int i =0; i < num_blocks; i++) seg_summary[i]->clearSegSum();

    current_segment++;
}

int WriteBuffer::getNumBlocks(){
    return num_blocks;
}

/*
   Block* WriteBuffer::createImapBlock()
   {
   Block *imap_block = new Block(2);
//imap_block.addInodeNum(currentIndex); // DO THIS
return imap_block;
}
*/
Block* WriteBuffer::getBlock(int idx){
    return buf[idx];
}

//-----------wbuffer segsum-----------//

void WriteBuffer::addSegInfo(block_num * data_blocks, block_num inode_block)
{
    for(int i = 0; i < MAX_FILE_BLOCKS; i++)
    {
        if(data_blocks[i] == 0) break;
        seg_summary[(data_blocks[i])/(BLOCK_SZ/8)]->addBlockNumSum(data_blocks[i], inode_block);
    }
}






//------------imaps---------------------//
void Block::addToCheckpointRegion(){
    
    Checkpoint_Region.imaps[Checkpoint_Region_counter++] = (BLOCK_SZ * current_segment)+ wbuffer.getNumBlocks(); 

}

void Block::addInodeData(int start_index)
{
    int iter = start_index;
    for(; iter < start_index+256; iter++)
    {
        if(g_imap.list[iter] == 0) break;
        block_data[iter-start_index] = g_imap.list[iter];
        cout <<"input value: " <<g_imap.list[iter] << "\n";
    }
    wbuffer.addBlock(this);
    addToCheckpointRegion();
}
