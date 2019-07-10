#include "_memory_.h"

MemoryModel *Memory_CreateModel(int realMemorySize, int swapMemorySize)
{// creates memory structures

	MemoryModel *mem = (MemoryModel*)malloc(sizeof(MemoryModel));   
	int i,j;
	mem->realMemorySize = realMemorySize;
	mem->swapMemorySize = swapMemorySize;

	mem->realMemory = (int**)malloc(realMemorySize*sizeof(int*));
	for(i=0; i<realMemorySize; i++)
		mem->realMemory[i] = (int*)malloc(realMemorySize*sizeof(int));
	mem->freeRealMemory = (int**)malloc(realMemorySize*sizeof(int*));
	for(i=0; i<realMemorySize; i++)
		mem->freeRealMemory[i] = (int*)malloc(realMemorySize*sizeof(int));

	mem->swapMemory = (int**)malloc(swapMemorySize*sizeof(int*));
	for(i=0; i<swapMemorySize; i++)
		mem->swapMemory[i] = (int*)malloc(swapMemorySize*sizeof(int));
	mem->freeSwapMemory = (int**)malloc(swapMemorySize*sizeof(int*));
	for(i=0; i<swapMemorySize; i++)
		mem->freeSwapMemory[i] = (int*)malloc(swapMemorySize*sizeof(int));

	// set all cell in free list to free (1)
	for(i=0; i<realMemorySize; i++)
		for(j=0; j<realMemorySize; j++) mem->freeRealMemory[i][j] = 1, mem->realMemory[i][j] = -1;
	for(i=0; i<swapMemorySize; i++)
		for(j=0; j<swapMemorySize; j++) mem->freeSwapMemory[i][j] = 1, mem->swapMemory[i][j] = -1;

	return mem;
}

int Memory_GetNumFreeFrames(MemoryModel *mem, char *_memType)
{
	int **memType;
	int memSize;
	int i, j;	
	int free = 0;

	memType = (int**)Memory_GetModelProperty(mem, _memType, "freelist");
	memSize = *((int*)Memory_GetModelProperty(mem, _memType, "size"));

	for(i=0; i<memSize; i++)
		for(j=0; j<memSize; j++)
			if(memType[i][j] == 1) free++;

#ifdef _DEBUG_ 
	printf("Memory_GetNumFreeFrames: %s has %d free cells\n", _memType, free);
#endif

	return free;
}

void Memory_PrintUsedList(MemoryModel *mem, char *_memType, FILE *out)
{
	int **memType, **freeMemType;
	char *memName;
	int memSize;
	int i, j;	  
	int usedSpace;

	memType = (int**)Memory_GetModelProperty(mem, _memType, "contents");
	freeMemType = (int**)Memory_GetModelProperty(mem, _memType, "freelist");
	memName = (char*)Memory_GetModelProperty(mem, _memType, "name");	
	memSize = *((int*)Memory_GetModelProperty(mem, _memType, "size"));
	usedSpace = (memSize*memSize)-Memory_GetNumFreeFrames(mem, _memType);

	fprintf(out, "\nUsed Space in %s:\n", memName);
	fprintf(out, "%d Frames used.\n", usedSpace);
	fprintf(out, "|----------|--------------------|\n");
	fprintf(out, "|%-10s|%-20s|\n", "Frame#", "Contents (pIDs)");
	fprintf(out, "|----------|--------------------|\n");
	for(i=0; i<memSize; i++)
		for(j=0; j<memSize; j++)
			if(freeMemType[i][j] == 0) 
				fprintf(out, "|%-10d|%-20d|\n", i*memSize+j, memType[i][j]);
	fprintf(out, "|----------|--------------------|\n");
}

void Memory_PrintFreeList(MemoryModel *mem, char *_memType, FILE *out)
{
	int **memType, **freeMemType;
	char *memName;
	int memSize;
	int i, j;	  
	int freeSpace;

	memType = (int**)Memory_GetModelProperty(mem, _memType, "contents");
	freeMemType = (int**)Memory_GetModelProperty(mem, _memType, "freelist");
	memName = (char*)Memory_GetModelProperty(mem, _memType, "name");	
	memSize = *((int*)Memory_GetModelProperty(mem, _memType, "size"));
	freeSpace = Memory_GetNumFreeFrames(mem, _memType);

	fprintf(out, "\nFree Space in %s:\n", memName);
	fprintf(out, "%d Free Frames\n", freeSpace);
	fprintf(out, "|----------|-------------------------|\n");
	fprintf(out, "|%-10s|%-25s|\n", "Frame#", "(Inconsistent) Contents");
	fprintf(out, "|----------|-------------------------|\n");
	for(i=0; i<memSize; i++)
		for(j=0; j<memSize; j++)
			if(freeMemType[i][j] == 1) 
				fprintf(out, "|%-10d|%-25d|\n", i*memSize+j, memType[i][j]);
	fprintf(out, "|----------|-------------------------|\n");
}

void Memory_PrintMatrix(MemoryModel *mem, char *_memType, FILE *out)
{
	int **memType, **freeMemType;
	char *memName;
	int memSize;
	int i, j;	  
	int freeSpace, usedSpace;

	memType = (int**)Memory_GetModelProperty(mem, _memType, "contents");
	freeMemType = (int**)Memory_GetModelProperty(mem, _memType, "freelist");
	memName = (char*)Memory_GetModelProperty(mem, _memType, "name");	
	memSize = *((int*)Memory_GetModelProperty(mem, _memType, "size"));
	freeSpace = Memory_GetNumFreeFrames(mem, _memType);
	usedSpace = (memSize*memSize)-Memory_GetNumFreeFrames(mem, _memType);

	fprintf(out, "\n%s state\n", memName);
	fprintf(out, "Total: %d frames\n\n", memSize*memSize);	
	fprintf(out, "Free list (1 = FREE, 0 = USED): %d free frames\n", freeSpace);
	for(i=0; i<memSize; i++)
	{
		for(j=0; j<memSize; j++)
			fprintf(out, "%-2d|", freeMemType[i][j]);
		fprintf(out, "\n");
	}

	fprintf(out, "Frame contents (pIDs): %d used frames:\n", usedSpace);
	for(i=0; i<memSize; i++)
	{
		for(j=0; j<memSize; j++)
			fprintf(out, "%-2d|", memType[i][j]);
		fprintf(out, "\n");
	}
}

int Memory_AddPage(MemoryModel *mem, char *_memType, int pageContent)
{
	int **memType, **freeMemType;
	int memSize;
	int i, j;	  

	memType = (int**)Memory_GetModelProperty(mem, _memType, "contents");
	freeMemType = (int**)Memory_GetModelProperty(mem, _memType, "freelist");
	memSize = *((int*)Memory_GetModelProperty(mem, _memType, "size"));

	for(i=0; i<memSize; i++)
		for(j=0; j<memSize; j++)
			if(freeMemType[i][j] == 1)
			{
				freeMemType[i][j] = 0;
				memType[i][j] = pageContent;
				return i*memSize+j; // return frame index of the page
			}

			return -1; // target memory was full   
}


void Memory_RemovePage(MemoryModel *mem, char *_memType, int pageContent)
{
	int **memType, **freeMemType;
	int memSize;
	int i, j;	  

	memType = (int**)Memory_GetModelProperty(mem, _memType, "contents");
	freeMemType = (int**)Memory_GetModelProperty(mem, _memType, "freelist");
	memSize = *((int*)Memory_GetModelProperty(mem, _memType, "size"));

	for(i=0; i<memSize; i++)
		for(j=0; j<memSize; j++)
			if(freeMemType[i][j] == 0
				&& memType[i][j] ==  pageContent)		
			{
				memType[i][j] = -1;
				freeMemType[i][j] = 1;
				return;
			}	
}

void Memory_RemoveProcess(MemoryModel *mem, Process *process)
{
	int i;
	for(i = 0; i<process->numPages; i++)
		if(process->table[i].P == 1) //Memory_RemovePage(mem, "real", process->pID);
		{
			mem->realMemory[process->table[i].frame/mem->realMemorySize][process->table[i].frame%mem->realMemorySize] = -1;
			mem->freeRealMemory[process->table[i].frame/mem->realMemorySize][process->table[i].frame%mem->realMemorySize] = 1;
		}
		else Memory_RemovePage(mem, "swap", process->pID);
}

void *Memory_GetModelProperty(MemoryModel *mem, char *memType, char *property)
{
	if( strcmp(memType, "real") == 0)
	{
		if( strcmp(property, "contents") == 0) return (void *)mem->realMemory;
		else if( strcmp(property, "freelist") == 0) return (void *)mem->freeRealMemory;
		else if( strcmp(property, "name") == 0) return (void*)"Real Memory";
		else return (void *)&mem->realMemorySize;
	}
	else
	{
		if( strcmp(property, "contents") == 0) return (void *)mem->swapMemory;
		else if( strcmp(property, "freelist") == 0) return (void *)mem->freeSwapMemory;
		else if( strcmp(property, "name") == 0) return (void*)"Swap Memory";
		else return (void *)&mem->swapMemorySize;
	}
}

