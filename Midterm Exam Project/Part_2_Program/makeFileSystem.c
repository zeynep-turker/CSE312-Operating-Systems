#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
//#pragma pack(1)

struct SingleBlock{
	uint64_t *array[6];
};
struct DoubleBlock{
	uint64_t *array[6];
	struct SingleBlock next1;
	struct SingleBlock next2;
};
struct TripleBlock{
	uint64_t *array[6];
	struct DoubleBlock next1;
	struct DoubleBlock next2;
};
struct inode{
	uint64_t inodeNumber;
	uint64_t  size;
	char name[90];
	char time[102];
	uint64_t *direct[10];
	struct SingleBlock singleIndirect;
	struct DoubleBlock doubleIndirect;
	struct TripleBlock tripleIndirect;
};
struct superblock{
	uint64_t MagicNumber; // File system magic number
    uint64_t Blocks;      // Number of blocks in file system
    uint64_t InodeBlocks; // Number of blocks reserved for inodes
	uint16_t freeInode;   //Number of free inodes
	uint16_t freeBlockNumber;//Number of free blocks
	uint32_t **freeBlocks;//free blocks
	uint32_t **dataBlocks;//data blocks
	struct inode *iNode;//inodes
	struct inode *freeINodes;//free inodes
	int blockSize;//block size
};
struct dir{
     struct inode *iNode;
     char filename[60];
};
struct dir *directories;
struct superblock *super;
FILE *fp; //file descriptor
int fs_init(char* path);
void initINode(struct inode *iNode);

int main(){
	super = (struct superblock*)malloc(sizeof(struct superblock));
	char str[256]; 
	char *split;
	char command[100];
	int number1;
	int number2;
	char fileSystemName[100];

  	scanf("%[^\n]%*c", str);
	split = strtok(str," ");
	strcpy(command,split);
	split = strtok (NULL, " ");
	number1 = atoi(split);
	split = strtok (NULL, " ");
	number2 = atoi(split);
    split = strtok (NULL, " ");
	strcpy(fileSystemName,split);

	if(number1<0 || number2<0){
		printf("Numbers should not be less than zero.\n");
		return 0;
	}
	(*super).blockSize = 256*number1;
	(*super).freeInode = number2;
	int result;
	if(strcmp(command,"makeFileSystem") == 0)
		result = fs_init(fileSystemName);
	if (result == 1)
		printf("File System successfully initialized\n");
	else
		printf("File System initialization failed\n");

	return 0;
}
//File System Function
int fs_init(char* path){
	fp =fopen(path , "wb" );
	int written=0;
	super->InodeBlocks = 0;
	super->MagicNumber = 0;
	super->Blocks = 0;
	super->InodeBlocks = 0;
	//inodes
	(*super).iNode = (struct inode*)malloc(((*super).freeInode)*sizeof(struct inode));
	//free inodes
	(*super).freeINodes = (struct inode*)malloc(((*super).freeInode)*sizeof(struct inode));
	//inodes and free inodes is initiaiized
	for(uint64_t i=0;i<(*super).freeInode;++i){
		initINode(&(((*super).iNode)[i]));
	}
	for(uint64_t i=0;i<(*super).freeInode;++i){
		initINode(&(((*super).freeINodes)[i]));
	}
	//super block is written in file 
	written += fwrite(super,1, sizeof(struct superblock), fp);
		
	if(written >= 1048576){
		printf("File size cannot be larger than 1 Mb.\n");
		return 0;
	}
	fseek(fp,sizeof(struct superblock),SEEK_SET);
	int total = sizeof(struct superblock);
	//inodes and free inodes are written in file
	for(uint64_t i=0;i<(*super).freeInode;++i){
		written += fwrite(&(((*super).iNode)[i]),1, sizeof(struct inode), fp);
		if(written >= 1048576){
			printf("File size cannot be larger than 1 Mb.\n");
			return 0;
		}
		total += sizeof(struct inode);
		fseek(fp,total,SEEK_SET);
	}
	for(uint64_t i=0;i<(*super).freeInode;++i){
		written += fwrite(&(((*super).freeINodes)[i]),1, sizeof(struct inode), fp);
		if(written >= 1048576){
			printf("File size cannot be larger than 1 Mb.\n");
			return 0;
		}
		total += sizeof(struct inode);
		fseek(fp,total,SEEK_SET);
	}
	int bsize = (int) (1048576 - written) / (4*((*super).blockSize));
	bsize = bsize/2;
	if(bsize <= 0){
		printf("Block size is too large.\n");
		return 0;
	}
	(*super).Blocks = 0;
	(*super).freeBlockNumber = bsize/2;
	//data blocks and free blocks are created.
	(*super).freeBlocks = (uint32_t**)malloc((bsize/2)*sizeof(uint32_t));
	(*super).dataBlocks = (uint32_t**)malloc((bsize/2)*sizeof(uint32_t));
	for(int i=0; i<(bsize/2); ++i){
		((*super).dataBlocks)[i] = (uint32_t*)malloc(((*super).blockSize)*sizeof(uint32_t));
		((*super).freeBlocks)[i] = (uint32_t*)malloc(((*super).blockSize)*sizeof(uint32_t));
	}
	//data blocks and free blocks are initialized and written in file.
	for (int i=0;i<(bsize/2);i++){
		for(int j=0; j<(*super).blockSize; ++j)
			((*super).dataBlocks)[i][j] = 0;
		written += fwrite(((*super).dataBlocks)[i],1,4*((*super).blockSize),fp);
		if(written >= 1048576){
			printf("File size cannot be larger than 1 Mb.\n");
			return 0;
		}
		total += 4*((*super).blockSize);
		fseek(fp,total,SEEK_SET);
	}
	for (int i=0;i<(bsize/2);i++){
		for(int j=0; j<(*super).blockSize; ++j)
			((*super).freeBlocks)[i][j] = 0;
		written += fwrite(((*super).freeBlocks)[i],1,4*((*super).blockSize),fp);
		if(written >= 1048576){
			printf("File size cannot be larger than 1 Mb.\n");
			return 0;
		}
		total += 4*((*super).blockSize);
		fseek(fp,total,SEEK_SET);
	}
	bsize = (int) (1048576 - written) / 64; 
	//directory entries are created.
	directories = (struct dir*)malloc(bsize*sizeof(struct dir));
	//root created.
	directories[0].iNode = &(((*super).iNode)[0]);
	directories[0].iNode->inodeNumber = 1;
	directories[0].iNode->name[0] = '.';
	directories[0].iNode->name[1] = '\0';
	directories[1].iNode = &(((*super).iNode)[1]);
	directories[1].iNode->inodeNumber = 1;
	directories[1].iNode->name[0] = '.';
	directories[1].iNode->name[1] = '.';
	directories[1].iNode->name[2] = '\0';
	((*super).InodeBlocks)++;
	((*super).InodeBlocks)++;
	for(int i=0; i<bsize; ++i){
		written += fwrite(&directories[i],1,sizeof(directories[i]),fp);
		if(written >= 1048576){
			printf("File size cannot be larger than 1 Mb.\n");
			return 0;
		}
		total += sizeof(directories[i]);
		fseek(fp,total,SEEK_SET);
	}
	fclose(fp);
	//free
	free(directories);
	free(super->iNode);
	free(super->freeINodes);
	free((*super).dataBlocks);
	free((*super).freeBlocks);
	free(super);
	
	return 1;
}
//initialize inode
void initINode(struct inode *iNode){
	iNode->inodeNumber = 0;
	iNode->size = 0;
	strcpy(iNode->name,"\0");
	strcpy(iNode->time,"\0");
	for(int i=0; i<10; ++i)
		iNode->direct[i] = 0;
	//single indirect	
	for(int i=0; i<6; ++i)
		iNode->singleIndirect.array[i] = 0;
	//double indirect
	for(int i=0; i<6; ++i) {
		iNode->doubleIndirect.array[i] = 0;
		iNode->doubleIndirect.next1.array[i] = 0;
		iNode->doubleIndirect.next2.array[i] = 0;
	}
	//triple indirect
	for(int i=0; i<6; ++i) {
		iNode->tripleIndirect.array[i] = 0;
		iNode->tripleIndirect.next1.array[i] = 0;
		iNode->tripleIndirect.next1.next1.array[i] = 0;
		iNode->tripleIndirect.next1.next2.array[i] = 0;
		iNode->tripleIndirect.next2.array[i] = 0;
		iNode->tripleIndirect.next2.next1.array[i] = 0;
		iNode->tripleIndirect.next2.next2.array[i] = 0;	
	}
}
