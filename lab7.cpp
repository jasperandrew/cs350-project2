#include "lab7.h"

int main(int argc, char **argv)
{
	string path = "DRIVE";
	mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	path += "/SEGMENT";
	for(int i = 0; i < 32; i++){
		mkdir((path + to_string(i+1)).c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	}
	return 0;
}