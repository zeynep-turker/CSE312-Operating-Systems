#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int main(int argc, char *argv[]) 
{
	if(argc != 3){
		printf("Wrong Argument Number\n");
		return 0;
	}
	int virtualMemCapacity = atoi(argv[1]);
	int numPhysical = atoi(argv[2]);
	numPhysical = pow(2,numPhysical);
	int frameSize=1;
	
	//turn till found best frame size
	while(numPhysical*frameSize < virtualMemCapacity){
		frameSize++;
		if(numPhysical*frameSize > virtualMemCapacity){
			frameSize--;
			break;
		}

	}
	printf("Best Frame Size is %d.\n",frameSize);
	
	
	
	return 0;
}
