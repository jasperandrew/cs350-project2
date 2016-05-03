#ifndef LAB7_H_
#define LAB7_H_

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace boost::algorithm;

int import(char * filename, char * lfs_filename);

int initDrive();

#endif
