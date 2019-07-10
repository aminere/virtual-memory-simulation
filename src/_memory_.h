#ifndef __memory_h_
#define __memory_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strcmp() */
#include "debug.h"

#include "process.h"

typedef struct _MemoryModel
{// Represents memory entites in computer (Real Memory and Disk)
	int realMemorySize;
	int swapMemorySize;

    int **realMemory;
    int **swapMemory;
	
	// free lists
	int **freeRealMemory; // holds info wheter a cell is free (just 0 or 1)
	int **freeSwapMemory; // same but for swap area*/
}MemoryModel;

MemoryModel* Memory_CreateModel(int realMemorySize, int swapMemorySize);

int Memory_GetNumFreeFrames(MemoryModel *mem, char *_memType);

int Memory_AddPage(MemoryModel *mem, char *_memType, int pageContent);

void Memory_RemovePage(MemoryModel *mem, char *_memType, int pageContent);

void Memory_RemoveProcess(MemoryModel *mem, Process *process);

void *Memory_GetModelProperty(MemoryModel *mem, char *memType, char *property);

void Memory_PrintUsedList(MemoryModel *mem, char *_memType, FILE *out);
void Memory_PrintFreeList(MemoryModel *mem, char *_memType, FILE *out);
void Memory_PrintMatrix(MemoryModel *mem, char *_memType, FILE *out);

#endif
