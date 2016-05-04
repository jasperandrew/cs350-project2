#include "lab7.h"

int main(int argc, char **argv)
{
	int reload = initDrive();
	
	import("test_file.txt", "test_file");
	
	wbuffer.writeToDisk();
	
	ifstream segment("DRIVE/SEGMENT1", ios::in | ios::binary);
	for(int i = 0; i < 30+8; i++){
		segment.seekg(1024*i);
		char block[1024] = {0};
		segment.read(block, 1024);
		cout << "[[BLOCK" << i+1 << "]]\n" << block << endl;
	}

	return 0;
}