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
	
	cout << "\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n";
	
	WriteBuffer wb;
	
	wb.addBlock(a);
	wb.addBlock(b);
	wb.addBlock(c);
	wb.addBlock(i);
	wb.addBlock(m);
		
	wb.writeToDisk();
	
	ifstream segment("DRIVE/SEGMENT1", ios::in | ios::binary);
	for(int i = 0; i < 5; i++){
		segment.seekg(1024*i);
		char stuf[1024] = {0};
		segment.read(stuf, 1024);
		cout << stuf << endl;
	}

	return 0;
}