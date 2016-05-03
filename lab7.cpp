#include "lab7.h"

int main(int argc, char **argv)
{
	string path = "DRIVE";
	mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	path += "/SEGMENT";
	for(int i = 0; i < 32; i++){
		FILE *fp = fopen((path + to_string(i+1)).c_str(), "w");
		ftruncate(fileno(fp), 1024*1024);
		fclose(fp);
	}
	return 0;
}