#include "lab7.h"

int main(int argc, char **argv)
{
	int reload = initDrive();
	
	Block a, b, c, i, m;
	char stuff[1024];
	
	i.setType(1);
	m.setType(2);
	
	ifstream fina("README.md", ios::in | ios::binary );
	fina.read(stuff, 1024);
	a.setData(stuff);
	fina.close();
	
	for(int i = 0; i < 1024; i++) stuff[i] = 0;
	ifstream finb("makefile", ios::in | ios::binary );
	finb.read(stuff, 1024);
	b.setData(stuff);
	finb.close();
	
	for(int i = 0; i < 1024; i++) stuff[i] = 0;
	ifstream finc("test.cpp", ios::in | ios::binary );
	finc.read(stuff, 1024);
	c.setData(stuff);
	finc.close();

	i.setFilename("FlimFlamJimJam");
	i.addFileBlock('0');
	i.addFileBlock('1');
	i.addFileBlock('2');
	
	m.addInodeNum('7');
	
	cout << a.getData() << endl;
	cout << "\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n";
	cout << b.getData() << endl;
	cout << "\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n";
	cout << c.getData() << endl;
	cout << "\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n";
	cout << i.writeInode() << endl;
	cout << "\n\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n";
	cout << m.writeImap() << endl;
	
	/*
	WriteBuffer wb;
	
	do{
		wb.addBlock(b);
	}while(wb.getNumBlocks() > 0);
	
	for(int i = 0; i < 512; i++){
		wb.addBlock(b);
	}
	cout << wb.getBlock(511).getData() << endl;
	
	wb.writeToDisk();
	*/
	return 0;
}