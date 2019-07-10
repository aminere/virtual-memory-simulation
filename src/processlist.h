#ifndef __processlist_h_
#define __processlist_h_

#include "process.h"
#include "_memory_.h"
#include "math.h"

typedef struct _ProcessList
{ // Holds an array of Processes and its size
  Process **process;
  int numProcesses;
  int maxProcesses;
}ProcessList;

// Process List Funcs
ProcessList *ProcessList_Create(int maxProcesses);

PROCESS_ERROR	 ProcessList_AddProcess(ProcessList *list, MemoryModel *mem,
									int pID, int numPages, int residentSetSize);

Process*	 ProcessList_GetProcessWithFittingResidentSet(ProcessList *list, int minSet);

int			 ProcessList_RemoveProcess(ProcessList *list, MemoryModel *mem, int pID);

Process*	 ProcessList_GetProcess(ProcessList *list, int pID);

int			 ProcessList_GetProcessIndex(ProcessList *list, int pID);

int			 ProcessList_RunProcess(ProcessList *list, MemoryModel *mem,
									int pID, int pageToRequest);

#endif