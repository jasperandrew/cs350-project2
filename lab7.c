//Daelin Fischman, Jasper Andrew and Jacob Zwickler 

#include <stdio.h>
#include <stdlib.h>




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


void validateArgs(char* arg1, char* arg2)
{
  if(arg1  == NULL || arg2  == NULL)
    {
      fprintf(stderr, "forgot input file name or number of frames\n");
      exit(0);
    }
  return;
} 

