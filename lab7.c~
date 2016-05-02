//Jasper Andrew and Jacob Zwickler

#include <stdio.h>
#include <stdlib.h>
#include "lab5.h"

#define LIMIT 1000

int main(int argc, char** argv)
{
  validateArgs(argv[1],argv[2]);
  freeSpace = atoi(argv[1]);
  MAXSPACE = freeSpace;
  char* fileName = argv[2];
  
  initializeList();

  char commandCheck[12];
  //param2 can be address size or vpn depending on instruction
  int procNum, param2 = 0;

  FILE *input = fopen(fileName, "r");
                                                                                                                              
  while(fscanf(input, "%s", commandCheck) != EOF)
    {
      if(!strncmp(commandCheck, "START", 2))
	{
	  fscanf(input, "%d %d\n", &procNum, &param2);
	  printf("Starting process %d with address size %d\n",procNum, param2);
	  start(procNum, param2);
	}
      else if(!strncmp(commandCheck, "TERMINATE", 2))
        {
	  fscanf(input, "%d\n", &procNum);
	  printf("terminating process %d\n", procNum);
	  terminate(procNum);        
	}
      else if(!strncmp(commandCheck, "REFERENCE", 2))
        {
	  fscanf(input, "%d %d\n", &procNum, &param2);
          printf("Referencing process %d's page %d\n",procNum, param2);
	  refer(procNum, param2);
	}      
    }
  close(input);
  return 0;
}

void initializeList()
{
  //start program with no processes running
  int i;
  for(i = 0; i < LIMIT; i++)
    {
      procList[i] = 0;
    } 
  return;
}

/*----Start---*/
void start(int procNum, int addrSz)
{
  int i = 0;
  int k = 0;
  
  //search if process already running 
  int valid =  validateProc(procNum, 's');
  if(valid >= 0)
    {
      //search if limit for processes reached - 
      while(procList[i] != 0)
	{
	  if(i<LIMIT)
	    {
	      i++;
	    }
	  else
	    {
	      fprintf(stderr, "Too many processes running. Limit is 1000!\n");
	      return;
	    }
	}   
      //place process in process list
      procList[i] = procNum;
  
      //create an "adress space" for the process at same index as process list
      int* temp  = malloc(sizeof(int)*(addrSz+1));
      pageTables[i] = temp;
  
      //set address space size  
      pageTables[i][0] = addrSz+1;
  
      /*
      //set valid bits for each page to valid  (bit 0)
      for(k = 1;k < addrSz+1; k++)
      {
      if(freeSpace > 0)
      {
      pageTables[i][k] = 0x00000001; 
      //update free frames
      freeSpace--;
      //	  printf("%x",pageTables[i][k]);
      }
      else
      {
      fprintf(stderr, "Not enough memory. All programs terminated.\n");
      i = 0;
      exit(0);
      }
      }*/ 
    }
  return;
}

/*--terminate---*/

void terminate(int procNum)
{
  int i = 0;
  int k = 0;
  int valid = validateProc(procNum, 't');
  if (valid >= 0)  
    {  
      while(procList[i] != procNum)
	{
	  i++;
	}
      //get address space size
      int length = pageTables[i][0];  
      for(k = 1;k < length; k++)
	{
	  //pageTables[i][k] = 0x00000000;
	  //update free frames
	  if(freeSpace < MAXSPACE && pageTables[i][k] == 0x1)
	    {
	      freeSpace++;
	    }
	}
      free(pageTables[i]);
      //printf("free frames: %d\n", freeSpace);
      printf("free frames: %d\n", freeSpace);
    }
  return;
}


/*------ Reference-------*/
void refer(int procNum, int  vpn)
{
  int procListIndex = validateProc(procNum, 't');
  if(procListIndex >=  0)
    {
      if((pageTables[procListIndex][0]-1) <  vpn || (pageTables[procListIndex][vpn] != 0x1) || freeSpace == 00)
	{
	  faults++;
	}
      if(pageTables[procListIndex][vpn] != 0x1 && freeSpace > 0)
	{
	  pageTables[procListIndex][vpn] = 0x1; 
	  freeSpace--;
	}
      totalRef++;
      fRate = (faults/totalRef);
      printf("free frames: %d\n", freeSpace);
      printf("faults total: %d  references total: %.0f fault rate: %.4f\n", faults, totalRef, fRate);
    }
  return;
}


/*------Validation----*/
int  validateProc(int procNum, char mode)
{
  int i;
  //search if process exists to terminate  
  if(mode == 't') 
    {
      i = 0;
      while(procList[i] != procNum)
	{
	  if(i < LIMIT)
	    {
	      i++;
	    }
	  else
	    {
	      fprintf(stderr, "Processes does not exist! No process to terminate.\n");
	      return -1;
	    }
	}
    }
  //search if process is already running 
  else if(mode == 's')
    {
      for(i = 0; i<LIMIT; i++)      
	{
	  if (procList[i] == procNum)
	    {
	      fprintf(stderr, "Processes already exists!\n");
	      return -1;
	    }
        
        }
    }
    return i;
}

void validateArgs(char* arg1, char* arg2)
{
  if(arg1  == NULL || arg2  == NULL)
    {
      fprintf(stderr, "forgot input file name or number of frames\n");
      exit(0);
    }
  return;
} 

