#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <math.h>
#include <pthread.h>
#include <alloca.h>
#include <unistd.h>
#include <semaphore.h>

//set and get function
void set(unsigned int index, int value, char * tName);
int get(unsigned int index, char * tName);
//Threads that make sorting
void *bubbleSort(void *args);
void *mergeSort(void *args);
void *quickSort(void *args);
void *indexSort(void *args);
//helpful function for sorting
void quicksort(int first,int last, int index);
void mergeSortt(int l, int r, int index);
void merge(int l, int m, int r, int index);
//make page replacement as LRU,NRU,FIFO,SC and WSClock
int pageReplacement(int value);
//prints the page table on the screen at every pageTablePrintInt memory accesses
void pageTablePrint();
//virtual and physical memory
int **physicalMemory;
int **virtualMemory;
//physical memory size
int physicalSize=0;
//this array hold all values
int *address;
//frame Size,
int frameSize;
//number of physical frames
int numPhysical;
//number of virtual frames
int numVirtual;
int pageTablePrintInt;
//bounds for every sorting
int bubbleN,mergeN,quickN,indexN;
//disk file
FILE *backing_store;
//disk file size
int fileSize=0;
//number of page faults,disk reads,disk writes
int pageFaults = 0;
int diskRead = 0;
int diskWrite = 0;
int writes = 0;
int reads = 0;
int pageRepl = 0;
int pageMiss = 0;
//frame index for fill,check,bubble,merge,index and quick
int fill=0,check=0;
int bubbleSet=0,mergeSet=0,indexSet=0,quickSet=0;
int bubbleGet=0,mergeGet=0,indexGet=0,quickGet=0;
int fifoA=0,fifoB=0;
int lruA=0, lruB=0;
int nruA=0, nruB=0;
int scA=0, scB=0;
int wsA=0, wsB=0;
//which page replacement
char *pagereplacement;
//disk file name
char *diskFileName;
FILE* temp;
int memoryAccesses = 0;
//used that increases memoryAccesses variable in all threads
sem_t sem;
//
int printInd=1;
int main(int argc, char *argv[])
{
	if(argc != 8){
		printf("Wrong Argument Number\n");
		return 0;	
	}
	frameSize = atoi(argv[1]);
	numPhysical = atoi(argv[2]);
	numVirtual = atoi(argv[3]);
	pagereplacement = argv[4];
	char *allocPolicy = argv[5];
	pageTablePrintInt = atoi(argv[6]);
	diskFileName = argv[7];
	
	if(frameSize<0 || numPhysical<0 || numVirtual<0){
		printf("Wrong Argument\n");
		return 0;
	}
	frameSize = pow(2,frameSize);
	numPhysical = pow(2,numPhysical);
	numVirtual = pow(2,numVirtual);
	bubbleN = numVirtual/4;
	quickN = bubbleN*2;
	mergeN = bubbleN*3;
	indexN = bubbleN*4;
	physicalMemory = (int**)malloc(sizeof(int*)*numPhysical);
	virtualMemory = (int**)malloc(sizeof(int*)*numVirtual);
	address = (int*)malloc(sizeof(int)*(frameSize*numVirtual));
	for(int i=0; i<numPhysical; ++i){
		physicalMemory[i] = (int*)malloc(sizeof(int)*frameSize);
	}
	for(int i=0; i<numVirtual; ++i){
		virtualMemory[i] = (int*)malloc(sizeof(int)*frameSize);
	}
	srand(1000);
	backing_store = fopen(diskFileName, "w");
    if (backing_store == NULL){
        fprintf(stderr, "Error opening %s\n",diskFileName);
        return -1;
    }
    if(sem_init(&sem,0,1) != 0){
			perror("Sem_init Error!\n");
			exit(EXIT_FAILURE);
	}
	//fill physical memory and disk file
	if(numVirtual >= numPhysical){
		int k=0;
		for(int i=0; i<numPhysical; ++i){
			for(int j=0; j<frameSize; ++j){
				int random = rand();
				physicalMemory[i][j] = random;
				address[k] = random;
				k++;
			}
			physicalSize++;
		}
		for(int i=frameSize*numPhysical; i<frameSize*numVirtual; ++i){
			int random = rand();
			fprintf(backing_store,"%d\n",random);
			address[k] = random;
			k++;
			fileSize++;
		}
	}
	else{
		int k=0;
		for(int i=0; i<numVirtual; ++i){
			for(int j=0; j<frameSize; ++j){
				int random = rand();
				physicalMemory[i][j] = random;
				address[k] = random;
				k++;
			}
			physicalSize++;
		}
	}
	//fill page table
	int k=0;
	for(int i=0; i<numVirtual; ++i){
		for(int j=0; j<frameSize; ++j){
			fill = i;
			set(j,address[k],"fill");
			k++;
			
		}
	}
	pthread_t Threads[4];
	int resize = 0;
	if(pthread_create(&Threads[0], NULL, bubbleSort, &resize) != 0){
	    perror("Pthread Create Error!\n");
		exit(EXIT_FAILURE);
	}
	if(pthread_create(&Threads[1], NULL, mergeSort, &resize) != 0){
	    perror("Pthread Create Error!\n");
		exit(EXIT_FAILURE);
	}
	if(pthread_create(&Threads[2], NULL, quickSort, &resize) != 0){
	    perror("Pthread Create Error!\n");
		exit(EXIT_FAILURE);
	}
	if(pthread_create(&Threads[3], NULL, indexSort, &resize) != 0){
	    perror("Pthread Create Error!\n");
		exit(EXIT_FAILURE);
	}
	for(int j=0; j<4; ++j){
		if(pthread_join(Threads[j],NULL) != 0){
			perror("Pthread Join Error!\n");
			exit(EXIT_FAILURE);
		}
	}
	//check if virtual memory is sorted
	int wrongSort=0;
	for(int i=0; i<numVirtual; ++i){
		check=i;
		for(int j=0; j<frameSize-1; ++j){
			for(int k=j+1; k<frameSize; ++k){
			if(get(j,"check") > get(k,"check"))
				wrongSort=1;
			}
		}
	}
	if(wrongSort == 1){
		printf("Sorting is unsuccessfull\n");
	}
	else{
		printf("Sorting is successfull\n");
	}
	printf("For fill :\n");
	printf("Number of reads : %d\n",reads);
	printf("Number of writes : %d\n",writes);
	printf("Number of page misses : %d\n",pageMiss);
	printf("Number of page replacements : %d\n",pageRepl);
	printf("Number of disk page writes : %d\n",diskWrite);
 	printf("Number of disk page reads : %d\n\n",diskRead);
 	printf("For bubble :\n");
	printf("Number of reads : %d\n",reads);
	printf("Number of writes : %d\n",writes);
	printf("Number of page misses : %d\n",pageMiss);
	printf("Number of page replacements : %d\n",pageRepl);
	printf("Number of disk page writes : %d\n",diskWrite);
 	printf("Number of disk page reads : %d\n\n",diskRead);
 	printf("For quick :\n");
	printf("Number of reads : %d\n",reads);
	printf("Number of writes : %d\n",writes);
	printf("Number of page misses : %d\n",pageMiss);
	printf("Number of page replacements : %d\n",pageRepl);
	printf("Number of disk page writes : %d\n",diskWrite);
 	printf("Number of disk page reads : %d\n\n",diskRead);
 	printf("For merge :\n");
	printf("Number of reads : %d\n",reads);
	printf("Number of writes : %d\n",writes);
	printf("Number of page misses : %d\n",pageMiss);
	printf("Number of page replacements : %d\n",pageRepl);
	printf("Number of disk page writes : %d\n",diskWrite);
 	printf("Number of disk page reads : %d\n\n",diskRead);
 	printf("For index :\n");
	printf("Number of reads : %d\n",reads);
	printf("Number of writes : %d\n",writes);
	printf("Number of page misses : %d\n",pageMiss);
	printf("Number of page replacements : %d\n",pageRepl);
	printf("Number of disk page writes : %d\n",diskWrite);
 	printf("Number of disk page reads : %d\n\n",diskRead);
 	printf("For check :\n");
	printf("Number of reads : %d\n",reads);
	printf("Number of writes : %d\n",writes);
	printf("Number of page misses : %d\n",pageMiss);
	printf("Number of page replacements : %d\n",pageRepl);
	printf("Number of disk page writes : %d\n",diskWrite);
 	printf("Number of disk page reads : %d\n",diskRead);
	
	
	for(int i=0; i<numPhysical; ++i){
		free(physicalMemory[i]);
	}
	for(int i=0; i<numVirtual; ++i){
		free(virtualMemory[i]);
	}
	free(physicalMemory);
	free(virtualMemory);
	free(address);
	
	
	
	return 0;
	
}
//print Page Table at every pageTablePrintInt memory accesses 
void pageTablePrint(){
	
	printf("Page Table:\n");
	for(int i=0;i<numVirtual;++i){
	printf("%d->",i);
		for(int j=0; j<frameSize;++j)
			printf("%d ",virtualMemory[i][j]);
		printf("\n");
	}
	printf("\n");
	printf("\n");

}
void set(unsigned int index, int value, char * tName){
	
	if(strcmp(tName,"fill") == 0){
		int found=0;
		for(int i=0; i<physicalSize; ++i){
			for(int j=0; j<frameSize; ++j){
				if(physicalMemory[i][j] == value){
					reads++;
					virtualMemory[fill][index] = value;
					writes++;
					found=1;
				}
			}
		}
		if(found == 0){
			pageMiss++;
			pageRepl++;
			fclose(backing_store);
			backing_store = fopen(diskFileName, "r");
			if (backing_store == NULL){
				fprintf(stderr, "Error opening %s\n",diskFileName);
				return;
			}
			for(int i=0; i<fileSize; ++i){
				int read;
				fscanf(backing_store,"%d",&read);
				if(read == value){
					virtualMemory[fill][index] = value;
					diskRead++;
					char *tempName = "temp.txt";
					temp = fopen(tempName, "w");
					if (temp == NULL){
						fprintf(stderr, "Error opening %s\n",tempName);
						return;
					}
					fseek(backing_store, 0, SEEK_SET);
					

					for(int j=0; j<fileSize; ++j){
						if(j == i){
							int write;
							fscanf(backing_store,"%d",&write);
							int result = pageReplacement(value);
							fprintf(temp,"%d\n",result);
							diskWrite++;
						}
						else{
							int write;
							fscanf(backing_store,"%d",&write);
							fprintf(temp,"%d\n",write);
						}
						
					}
					fclose(backing_store);
					remove(diskFileName);
					fclose(temp);
					temp = fopen(tempName, "r");
					if (temp == NULL){
						fprintf(stderr, "Error opening %s\n",tempName);
						return;
					}
					backing_store = fopen(diskFileName, "w");
					if (backing_store == NULL){
						fprintf(stderr, "Error opening %s\n",diskFileName);
						return;
					}
					for(int j=0; j<fileSize; ++j){
						int write;
						fscanf(temp,"%d",&write);
						fprintf(backing_store,"%d\n",write);
					}
					fclose(temp);
					remove(tempName);
					return;
				}
			}
		}
	}
	else if(strcmp(tName,"bubble")==0){
		if(sem_wait(&sem) != 0){
			perror("Sem_post Error!\n");
			exit(EXIT_FAILURE);
		}
		memoryAccesses++;
		if(memoryAccesses == (printInd*pageTablePrintInt)){
			pageTablePrint();
			printInd++;
		}
		if(sem_post(&sem) != 0){
			perror("Sem_post Error!\n");
			exit(EXIT_FAILURE);
		}
		virtualMemory[bubbleSet][index] = value;
	}
	else if(strcmp(tName,"quick")==0){
		if(sem_wait(&sem) != 0){
			perror("Sem_post Error!\n");
			exit(EXIT_FAILURE);
		}
		memoryAccesses++;
		if(memoryAccesses == (printInd*pageTablePrintInt)){
			pageTablePrint();
			printInd++;
		}
		if(sem_post(&sem) != 0){
			perror("Sem_post Error!\n");
			exit(EXIT_FAILURE);
		}
		virtualMemory[quickSet][index] = value;
	}
	else if(strcmp(tName,"merge")==0){
		if(sem_wait(&sem) != 0){
			perror("Sem_post Error!\n");
			exit(EXIT_FAILURE);
		}
		memoryAccesses++;
		if(memoryAccesses == (printInd*pageTablePrintInt)){
			pageTablePrint();
			printInd++;
		}
		if(sem_post(&sem) != 0){
			perror("Sem_post Error!\n");
			exit(EXIT_FAILURE);
		};
		virtualMemory[mergeSet][index] = value;
	}
	else if(strcmp(tName,"index")==0){
		if(sem_wait(&sem) != 0){
			perror("Sem_post Error!\n");
			exit(EXIT_FAILURE);
		}
		memoryAccesses++;
		if(memoryAccesses == (printInd*pageTablePrintInt)){
			pageTablePrint();
			printInd++;
		}
		if(sem_post(&sem) != 0){
			perror("Sem_post Error!\n");
			exit(EXIT_FAILURE);
		}
		virtualMemory[indexSet][index] = value;
	}
}
int get(unsigned int index, char * tName){
	if(sem_wait(&sem) != 0){
		perror("Sem_post Error!\n");
		exit(EXIT_FAILURE);
	}
	memoryAccesses++;
	if(memoryAccesses == (printInd*pageTablePrintInt)){
			pageTablePrint();
			printInd++;
		}
	if(sem_post(&sem) != 0){
		perror("Sem_post Error!\n");
		exit(EXIT_FAILURE);
	}
	if(strcmp(tName,"bubble")==0){
		return virtualMemory[bubbleGet][index];
	}
	else if(strcmp(tName,"quick")==0){
		return virtualMemory[quickGet][index];
	}
	else if(strcmp(tName,"merge")==0){
		return virtualMemory[mergeGet][index];
	}
	else if(strcmp(tName,"index")==0){
		return virtualMemory[indexGet][index];
	}
	else if(strcmp(tName,"check")==0){
		return virtualMemory[check][index];
	}
	return virtualMemory[check][index];
}
int pageReplacement(int value){
	if(strcmp(pagereplacement,"FIFO") == 0){
		int temp = physicalMemory[fifoA][fifoB];
		physicalMemory[fifoA][fifoB] = value;
		fifoB++;
		if(fifoB == frameSize){
			fifoB=0;
			fifoA++;
		}
		if(fifoA == numPhysical)
			fifoA=0;
		return temp;
	}
	else if(strcmp(pagereplacement,"LRU") == 0){
		int temp = physicalMemory[lruA][lruB];
		physicalMemory[lruA][lruB] = value;
		lruB++;
		if(lruB == frameSize){
			lruB=0;
			lruA++;
		}
		if(lruA == numPhysical)
			lruA=0;
		return temp;
	
	}
	else if(strcmp(pagereplacement,"NRU") == 0){
		int temp = physicalMemory[nruA][nruB];
		physicalMemory[nruA][nruB] = value;
		nruB++;
		if(nruB == frameSize){
			nruB=0;
			nruA++;
		}
		if(nruA == numPhysical)
			nruA=0;
		return temp;
	
	}
	else if(strcmp(pagereplacement,"SC") == 0){
		int temp = physicalMemory[scA][scB];
		physicalMemory[scA][scB] = value;
		scB++;
		if(scB == frameSize){
			scB=0;
			scA++;
		}
		if(scA == numPhysical)
			scA=0;		
		return temp;
	}
	else if(strcmp(pagereplacement,"WSClock") == 0){
		int temp = physicalMemory[wsA][wsB];
		physicalMemory[wsA][wsB] = value;
		wsB++;
		if(wsB == frameSize){
			wsB=0;
			wsA++;
		}
		if(wsA == numPhysical)
			wsA=0;
		return temp;
	}
	return 0;
}
//Bubble Sort Thread
void *bubbleSort(void *args){
	for(int k=0; k<bubbleN; ++k){
		int n = frameSize;
		int i, j, temp; 
	   	for (i = 0; i < n-1; i++){
			for (j = 0; j < n-i-1; j++){
				bubbleGet = k;
		    	if (get(j,"bubble") > get(j+1,"bubble")){
		    		temp = get(j,"bubble");
		    		bubbleSet = k;
					set(j,get(j+1,"bubble"),"bubble"); 
					set(j+1,temp,"bubble");
		      	}
		  	}
	  	}
  	}
	return NULL;
}
//Merge Sort Thread
void *mergeSort(void *args){
	for(int i=quickN; i<mergeN; ++i){
		mergeSortt(0,frameSize-1,i);
	}
	return NULL;
}
//Quick Sort Thread
void *quickSort(void *args){
	for(int i=bubbleN; i<quickN; ++i)
		quicksort(0,frameSize-1,i);
	return NULL;
}
//Index Sort Thread
void *indexSort(void *args){
	for(int k=mergeN; k<indexN; ++k){
		int *index = (int*)malloc(sizeof(int)*frameSize);
		int a=0;
		for(int i=0;i<frameSize;i++){
	  		index[a]=i;
	  		a++;
	 	}
		int m=0,n=0;
	 	for(int i=0;i<frameSize-1;i++){
	 		n=m+1;
	  		for(int j=i+1;j<frameSize;j++){
	   			int temp;
	   			indexGet = k;
			   	if(get(index[m],"index") > get(index[n],"index")){
					temp = index[m];
					index[m] = index[n];
					index[n] = temp;
			   	}
			   	n++;
		  	}
		  	m++;
		}
		m=0;
	 	for(int i=0;i<frameSize;i++){
	 		indexGet = k;
	 		indexSet = k;
			int temp = get(i,"index");
			set(i,get(index[m],"index"),"index");
			set(index[m],temp,"index");
			for(int j=0; j<frameSize; ++j){
				if(index[j] == i)
				    index[j] = index[m];
			}
			m++;
	 	}
	 	free(index);
 	}
	return NULL;
}
void quicksort(int first,int last,int index){
   	int i, j, pivot, temp;
	quickGet = index;
   	if(first<last){
      pivot=first;
      i=first;
      j=last;

      while(i<j){ 
         while(get(i,"quick") <= get(pivot,"quick")&&i<last)
            i++;
         while(get(j,"quick") > get(pivot,"quick"))
            j--;
         if(i<j){
            temp = get(i,"quick");
            quickSet = index;
			set(i,get(j,"quick"),"quick");
            set(j,temp,"quick");
         }
      }
	  quickSet = index;
      temp=get(pivot,"quick");
      set(pivot,get(j,"quick"),"quick");
      set(j,temp,"quick");
      quicksort(first,j-1,index);
      quicksort(j+1,last,index);

   }
}
void merge(int l, int m, int r,int index) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
    int left[n1], right[n2]; 
  
    mergeGet = index;
    for (i = 0; i < n1; i++) 
        left[i] = get(l+i,"merge"); 
    for (j = 0; j < n2; j++) 
        right[j] = get(m+1+j,"merge");

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) { 
        if (left[i] <= right[j]) {
        	mergeSet = index;
            set(k,left[i],"merge"); 
            i++;
        } 
        else { 
        	mergeSet = index;
        	set(k,right[j],"merge"); 
            j++; 
        } 
        k++; 
    } 
    while (i < n1) {
    	mergeSet = index;
        set(k,left[i],"merge"); 
        i++; 
        k++; 
    }
    while (j < n2) { 
    	mergeSet = index;
    	set(k,right[j],"merge"); 
        j++; 
        k++; 
    } 
} 
void mergeSortt(int l, int r,int index) 
{ 
    if (l < r) { 
        int m = l + (r - l) / 2;
        mergeSortt(l, m,index); 
        mergeSortt(m + 1, r,index); 
        merge(l, m, r,index); 
    } 
} 

