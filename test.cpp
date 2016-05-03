#include "lab7.h"

int main(int argc, char **argv)
{
	int reload = initDrive();
	
	Block b;
	char stuff[512];
	
	cout << b.getType() << endl;
	
	ifstream fin("test", ios::in | ios::binary );
	fin.read(stuff, 512);

	b.setData(stuff);
	cout << b.getData() << endl;
	
	WriteBuffer wb;
	
	do{
		wb.addBlock(b);
	}while(wb.getNumBlocks() > 0);
	
	for(int i = 0; i < 512; i++){
		wb.addBlock(b);
	}
	cout << wb.getBlock(511).getData() << endl;
	
	wb.writeToDisk();
	
	return 0;
}