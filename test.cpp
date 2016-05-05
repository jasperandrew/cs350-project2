#include "lab7.h"

int main(int argc, char **argv)
{
	int reload = initDrive();
	
	import("test_file.txt", "test_file");
	wbuffer.writeToDisk();
	
	import("test_file.txt", "test_file");
	wbuffer.writeToDisk();
	
	ifstream segment1("DRIVE/SEGMENT1", ios::in | ios::binary);
	for(int i = 33; i < 35; i++){
		segment1.seekg(1024*i);
		char block[1024] = {0};
		segment1.read(block, 1024);
		cout << "[[BLOCK" << i+1 << "]]\n" << block << endl;
	}
	segment1.close();

	ifstream segment("DRIVE/SEGMENT2", ios::in | ios::binary);
	for(int i = 33; i < 35; i++){
		segment.seekg(1024*i);
		char block[1024] = {0};
		segment.read(block, 1024);
		cout << "[[BLOCK" << i+1 << "]]\n" << block << endl;
	}
	segment.close();

	return 0;
}