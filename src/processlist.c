#include "processlist.h"

// PROCESS LIST FUNCS ///////////
////////////////////////////////

ProcessList *ProcessList_Create(int maxProcesses)
{
	ProcessList *list = (ProcessList*)malloc(sizeof(ProcessList));
	list->process = (Process**)malloc(maxProcesses*sizeof(Process*));
	list->maxProcesses = maxProcesses;
	list->numProcesses = 0;
	return list;
}


PROCESS_ERROR	ProcessList_AddProcess(ProcessList *list, MemoryModel *mem, 
									   int pID, int numPages, int residentSetSize)
{// Adds a process to the main list and allocates frames for it,
	// builds its page table, adjusts the free lists ...

	int i,j,k;
	int writtenPages = 0;
	Process *process;
	Process *smallestProcess;	
	int swapped;
	int lookForReference;

	// Check if pID exists
	for(i=0; i<list->numProcesses; i++)
		if(list->process[i]->pID == pID) return PROCESS_PID_EXISTS;

	// First check if it's possible to accept this process's resident set
	// in main memory. If not, check if there is a process that can
	// be swapped out to leave place for new resident set
	if((i = Memory_GetNumFreeFrames(mem, "real")) < residentSetSize)
	{
		if((smallestProcess = ProcessList_GetProcessWithFittingResidentSet(list, residentSetSize - i)) == NULL) 		
			return MEMORY_FULL; // didn't find any process to replace, REJECT new process
		if(Memory_GetNumFreeFrames(mem, "swap") < residentSetSize - i)
			return MEMORY_FULL; // swap area full, REJECT new process
	}
	else
		if(Memory_GetNumFreeFrames(mem, "swap") < numPages - residentSetSize)
		{
			// swap not enough. try expanding resident set
			while(residentSetSize < Memory_GetNumFreeFrames(mem, "real")
				&& residentSetSize < numPages) residentSetSize++;
		
			if(Memory_GetNumFreeFrames(mem, "swap") < numPages - residentSetSize)
				return MEMORY_FULL; // swap area still no enough for remaining pages
									// REJECT new process
		}

	// Now that it's sure that there is enough memory,
	// start the allocation of the new process
	// and return NOERROR at the end.
	process = Process_Create(pID, numPages, residentSetSize);
	list->process[list->numProcesses++] = process;

	// try put resident set in real mem	
	for(i=0; i<process->residentSetSize; i++)
	{
		j = Memory_AddPage(mem, "real", process->pID);
		if(j != -1) 
		{
			process->table[writtenPages].P = 1; // present
			process->table[writtenPages].R = 0;
			process->table[writtenPages].frame = j; 
			writtenPages++;
		}
		else break;		
	}

	if(writtenPages < process->residentSetSize)
	{ // Not all resident set pages written
		// swap out pages from smallest fitting process

		k = process->residentSetSize-writtenPages; // remaining Resident set pages

		printf("couldn't put whole resident set in memory.\n");
		printf("swapping out %d LRU pages from process with pID %d\n", k, smallestProcess->pID);

		// swap out pages with 0 reference first
		lookForReference = 0;
		for(i=0; i<k; i++)
		{
			swapped = 0;
			for(j=0; j<smallestProcess->numPages; j++)
			{
				if(smallestProcess->table[j].P == 1)
				{
					if(smallestProcess->table[j].R == lookForReference)
					{
						smallestProcess->residentSetSize--;
						smallestProcess->table[j].P = 0;
						j = smallestProcess->table[j].frame; // save frame# in j
						smallestProcess->table[j].frame = -1;

						// update page table
						process->table[writtenPages].P = 1; // present
						process->table[writtenPages].R = 0;
						process->table[writtenPages].frame = j; // frame# was saved in j
						writtenPages++;

						// update frame contents
						mem->realMemory[j/mem->realMemorySize][j%mem->realMemorySize] 
						= process->pID;

						Memory_AddPage(mem, "swap", smallestProcess->pID);			   

						swapped = 1;
						break;
					}
				}
			}
			if(swapped == 0) lookForReference = 1; // swap out referenced pages too
		}		
	}

	if(writtenPages == process->numPages) goto Done;

	// put remaining pages in swap memory
	for(i=writtenPages; i<process->numPages; i++)
	{
		process->table[i].P = 0; // not present
		process->table[i].R = 0;
		process->table[i].frame = -1; // not in real mem

		Memory_AddPage(mem, "swap", process->pID);		   
	}

Done: return NOERROR;
}


Process* ProcessList_GetProcessWithFittingResidentSet(ProcessList *list, int minSet)
{// returns process with (smallest) fitting resident set
	int i;
	Process *smallest = NULL;

	for(i = 0; i<list->numProcesses; i++)
	{
		if(list->process[i]->residentSetSize >= minSet)
		{
			if(smallest == NULL) smallest = list->process[i];
			else
				if(list->process[i]->residentSetSize < smallest->residentSetSize)
					smallest = list->process[i];
		}
	}

#ifdef _DEBUG_
	if(smallest)
		printf("ProcessList_GetProcessWithFittingResidentSet: fitting has set size %d\n", smallest->residentSetSize);
	else 
		printf("ProcessList_GetProcessWithFittingResidentSet: no fitting process found.\n");
#endif

	return smallest;
}


int	ProcessList_RemoveProcess(ProcessList *list, MemoryModel *mem, int pID)
{
	int i = ProcessList_GetProcessIndex(list, pID);  
	Process *process;

	int k;

	if(i != -1)
	{
		process = list->process[i];
		Memory_RemoveProcess(mem, process);

		// rearranges process array
		for(k=i; k<list->numProcesses-1; k++)
			list->process[k] = list->process[k+1];

		list->numProcesses--;

		Process_Destroy(process);
		return 1;
	}
	else return 0;   
}


int  ProcessList_RunProcess(ProcessList *list, MemoryModel *mem,
									int pID, int pageToRequest)
{ // return 1 on page fault

	int i;
	int lookForReference;
	Process *process;
	Process *smallest;

	process = ProcessList_GetProcess(list, pID);

	if(process->table[pageToRequest].P == 0)
	{
		printf("PAGE FAULT: page %d not loaded.\n", pageToRequest+1);

		process->table[pageToRequest].P = 1;
		process->table[pageToRequest].R = 1;					

		if(Memory_GetNumFreeFrames(mem, "real") < 1)
		{
			printf("Not enough memory to swap page in:\n");
			printf("Swapping out a page using LRU..\n");
			
			// try local replacement first
			if(process->residentSetSize > 0)
			{
			  lookForReference = 0; // look for null referenced pages first (LRU)
	  Repeat: for(i = 0; i< process->numPages; i++)
				{
					if(process->table[i].P == 1)
					if(process->table[i].R == lookForReference)
					{   
						process->table[pageToRequest].frame = process->table[i].frame;
						process->table[i].P = 0;
						process->table[i].R = 0;	
						process->table[i].frame = -1;
						printf("swapped page %d out.\n", i+1);
						// swap this page out
						return 1;
					}
				}	
				lookForReference = 1; // no page found. Look for referenced ones.
				goto Repeat;
			}
			else
			{
				printf("Not enough pages in local resident set.\n");
				printf("Replacing from global scope (from smallest process.)\n");
			
				smallest = ProcessList_GetProcessWithFittingResidentSet(list, 1);

				lookForReference = 0; // look for null referenced pages first
		Repeat2: for(i=0; i<smallest->numPages; i++)
				{
					if(smallest->table[i].P == 1)
					if(smallest->table[i].R == lookForReference)
					{   
						smallest->residentSetSize--;
						process->residentSetSize++;

						smallest->table[i].P = 0;
						smallest->table[i].R = 0;
						smallest->table[i].frame = -1;
                        						
						printf("swapped page %d out from process with pID %d.\n", i+1, smallest->pID);

						i = smallest->table[i].frame;

						process->table[pageToRequest].frame = i;

						// update frame contents
						mem->realMemory[i/mem->realMemorySize][i%mem->realMemorySize] 
						= process->pID;

						Memory_RemovePage(mem, "swap", process->pID);
						Memory_AddPage(mem, "swap", smallest->pID);			   

						return 1;
					}
				}	
				lookForReference = 1; // no page found. Look for referenced ones.
				goto Repeat2;				
			}			
		}
		else
		{
			printf("There's enough memory to swap page in (no swap out).\n");
			printf("Extending the Resident set.. ( + 1 )\n");

			i = Memory_AddPage(mem, "real", process->pID);
			process->table[pageToRequest].frame = i;
			process->residentSetSize ++;
			Memory_RemovePage(mem, "swap", process->pID);
		}

		return 1;
	}
	else 
	{
		printf("Page %d is in memory. (no page fault)\n", pageToRequest+1);
		return 0;
	}
}


Process* ProcessList_GetProcess(ProcessList *list, int pID)
{
	int i;

	for(i=0; i<list->numProcesses; i++)
		if(list->process[i]->pID == pID) 
			return list->process[i];

	return NULL;
}

int ProcessList_GetProcessIndex(ProcessList *list, int pID)
{
	int i;
	for(i=0; i<list->numProcesses; i++)
		if(list->process[i]->pID == pID)
			return i;

	return -1;
}


