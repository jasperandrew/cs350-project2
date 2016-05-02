#include "lab7.h"


vector< char[1024]> blockBuffer;

int lab7::import(char * filename, char * lfs_filename)
{

    ifstream inputFile(filename, ifstream::in);
    if(inputFile)
    {
        char * kbBlock = new char [1024];
        while(inputFile.get(kbBlock,1025,EOF) && blockBuffer.size()< 1024)
        {
            blockBuffer.push_back(kbBlock);
        }
        for(int i=0; i <blockBuffer.size(); i++)
        {
            cout <<blockBuffer[i] <<endl;
        }
    }    
}

int main(int argc, char **argv)
{
	return 0;
}




