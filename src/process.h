#ifndef __process_h_
#define __process_h_

#include <stdio.h>
#include <stdlib.h>
#include "pagetable.h"

typedef enum _PROCESS_ERROR
{
  PROCESS_LIST_FULL=0, PROCESS_PID_EXISTS, MEMORY_FULL, NOERROR 
}PROCESS_ERROR;

typedef struct _Process
{ // Defines a process image
  int pID;
  int numPages;
  int residentSetSize;
  PageTableEntry *table; // array of page table entries = page table
}Process;

// Process Funcs 
Process* Process_Create(int pID, int numPages, int residentSetSize);
void Process_Destroy(Process *process);
void Process_PrintPageTable(Process *process, FILE *out);
void Process_PrintResidentSet(Process *process, FILE *out);
void Process_FlipReferenceBits(Process *process);

char* Process_GetErrorMsg(PROCESS_ERROR error);

#endif