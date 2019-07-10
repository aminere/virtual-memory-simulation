#include "process.h"

#include <stdio.h>
#include <stdlib.h>

// PROCESS FUNCS ////////////////
/////////////////////////////////

Process* Process_Create(int pID, int numPages, int residentSetSize)
{// create new process structure, NO PAGE/FRAME allocation HERE.
 // USED by ProcessList_AddProcess(), which performs the 
 // correct page/frame allocation.

// SEE ProcessList_AddProcess() in processlist.c !!

	Process* p = (Process*)malloc(sizeof(Process));
	p->pID = pID;
	p->numPages = numPages;
	p->residentSetSize = residentSetSize;
	p->table = (PageTableEntry*)malloc(numPages*sizeof(PageTableEntry));
	return p;
}

void Process_Destroy(Process *process)
{// desallocates memory for a process,
 // specially the process table. 
 // THIS doesn't NOT DO PAGE/FRAME Modifications.
 // USED by ProcessList_RemoveProcess(), which performs the 
 // correct page/frame modifications

 // SEE ProcessList_RemoveProcess() in processlist.c !!
	free(process->table);
	free(process);
}

void Process_PrintPageTable(Process *process, FILE *out)
{
	int i;
	fprintf(out, "Process pID: %d\n", process->pID);
	fprintf(out, "PageTable:\n");
	for(i=0; i<process->numPages; i++)	 
		fprintf(out, "Page %-2d: P=%d|R=%d|Frame=%d\n",
		i+1,
		process->table[i].P,
		process->table[i].R,
		process->table[i].frame);
}

void Process_PrintResidentSet(Process *process, FILE *out)
{
	int i;
	fprintf(out, "Process pID: %d\n", process->pID);
	fprintf(out, "Pages in Resident Set:\n");
	for(i=0; i<process->numPages; i++)	
		if(process->table[i].P == 1)
		fprintf(out, "Page %d: P=%d|R=%d|Frame=%d\n",
		i+1,
		process->table[i].P,
		process->table[i].R,
		process->table[i].frame);
}

void Process_FlipReferenceBits(Process *process)
{
	int i;
	printf("flipping reference bits from process page table..\n");
	for(i =0; i<process->numPages; i++)
		if(process->table[i].P == 1)		
			process->table[i].R = rand()%2;		
}

char* Process_GetErrorMsg(PROCESS_ERROR error)
{
  if(error == PROCESS_LIST_FULL) return "Process List Full";
  else if(error == PROCESS_PID_EXISTS) return "Process pID Exists";
  else if(error == MEMORY_FULL) return "Not Enough Memory";
  else if(error == NOERROR) return "No Process Error Occured";
  else return "Unknow Process Error";
}

