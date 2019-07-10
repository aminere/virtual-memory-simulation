#include "simulation.h"

Simulation *Simulation_Create()
{
  Simulation *simulation = (Simulation*)malloc(sizeof(Simulation));
  return simulation;
}

void Simulation_Run(Simulation *simulation)
{
	int input;

	system(CLEAR_COMMAND);
	printf("starting sim..\n");

	input = Simulation_InputInteger("Enter max number of processes in simulation: ", 0);

	simulation->processList = ProcessList_Create(input);

	simulation->memoryModel = Memory_CreateModel(16, 32);

	while(1)
	{
		input = Simulation_DisplayMenu();

		if(input == 1) Simulation_CreateNewProcess(simulation);  
		else if(input == 2) Simulation_SimulateRunning(simulation);
		else if(input == 3) Simulation_DisplayProcessPageTable(simulation);
		else if(input == 4) Simulation_DisplayProcessResidentSet(simulation);
		else if(input == 5) Simulation_DisplayMemoryState(simulation, "real");
		else if(input == 6) Simulation_DisplayMemoryState(simulation, "swap");
		else if(input == 7) Simulation_KillProcess(simulation);
		else if(input == 8) break;
	}

	system(CLEAR_COMMAND);
	printf("Thanks for using this program.\n");
	printf("By Amine Rehioui, http://www.aui.ma/~A.Rehioui\n");
	printf("And Benjelloun Med Neyl\n");
	Simulation_Pause();
}


int Simulation_DisplayMenu()
{
	int input;

	while(1)
	{
		system(CLEAR_COMMAND);
		printf("Paging and V-Mem simulation.\n");
		printf("By Amine Rehioui and Med Neyl Benjelloun\n");
		printf("1 - Create New Process\n");
		printf("2 - Simulate Process Running (Referencement)\n");
		printf("3 - Display Proces Page Table\n");
		printf("4 - Display Process Resident Set\n");
		printf("5 - Display Real Memory State\n");
		printf("6 - Display Swap Memory State\n");
		printf("7 - Kill Process\n");
		printf("8 - Quit\n");
		printf("please make a choice: ");
		scanf("%d", &input);
		if(input < 1 || input > 8)
			printf("invalid choice\n"), Simulation_Pause();
		else return input;
	}
}

int Simulation_DisplaySubMenu(SUB_MENU submenu)
{
  int input;
  if(submenu == DISPLAY_MEMORY_STATE)
  {
    while(1)
	{
		system(CLEAR_COMMAND);
		printf("1 - Display Used space status.\n");
		printf("2 - Display Free space status.\n");
		printf("3 - Display Matrix representation. (USEFUL)\n");
		printf("4 - Back\n");
		printf("please make a choice: ");
		scanf("%d", &input);
		if(input < 1 || input > 4)
			printf("invalid choice\n"), Simulation_Pause();
		else return input;
	}
  }  
  else if(submenu == RUN_PROCESS)
  {
	while(1)
	{
		system(CLEAR_COMMAND);
		printf("1 - Automatic page referencement.\n");
		printf("2 - Manual page referencement.\n");
		printf("3 - Back\n");
		printf("please make a choice: ");
		scanf("%d", &input);
		if(input < 1 || input > 3)
			printf("invalid choice\n"), Simulation_Pause();
		else return input;
	}
  }
  else 
  {
	  printf("never going to be displayed: just not to make the compiler complain.\n"); 
	  return -1;
  }
}

void Simulation_CreateNewProcess(Simulation *simulation)
{
  int pID;
  int numPages;
  int residentSetSize;
  PROCESS_ERROR error;

  if(simulation->processList->numProcesses == simulation->processList->maxProcesses)
  {
	  printf("Could Not Create New Process: %s\n", Process_GetErrorMsg(PROCESS_LIST_FULL));
	  Simulation_Pause();
	  return;
  }

  pID = Simulation_InputInteger("enter the process pid: ", 1);
  numPages = Simulation_InputInteger("enter the process number of pages: ", 0);
  residentSetSize = Simulation_InputInteger("enter the process resident set size: ", 0);  

  error = ProcessList_AddProcess(simulation->processList,
	  simulation->memoryModel, pID, numPages, residentSetSize);

  if(error != NOERROR)
	  printf("Could Not Create New Process: %s\n", Process_GetErrorMsg(error));
  else
	  printf("New process added to the main process list.\n");

  Simulation_Pause();
}

void Simulation_DisplayProcessPageTable(Simulation *simulation)
{
   Process *process = Simulation_SelectProcess(simulation);
   FILE *out;

   if(process != NULL) 
   {
	   out = fopen("PROCESS.TXT", "wt");

	   Process_PrintPageTable(process, stdout);
	   Process_PrintPageTable(process, out);

	   fclose(out);

	   printf("\nSEE FILE PROCESS.TXT FOR FULL OUTPUT\n\n");
   }

   Simulation_Pause();
}

void Simulation_DisplayProcessResidentSet(Simulation *simulation)
{
   Process *process = Simulation_SelectProcess(simulation);
   if(process != NULL) Process_PrintResidentSet(process, stdout);

   Simulation_Pause();
}

void Simulation_DisplayMemoryState(Simulation *simulation, char *memType)
{
  int input;
  FILE *out;

  while(1)
  {
    input = Simulation_DisplaySubMenu(DISPLAY_MEMORY_STATE);
	
	if(input == 1)  
	{
	  out = fopen("MEM.TXT", "wt"); 
	  Memory_PrintUsedList(simulation->memoryModel, memType, stdout),
	  Memory_PrintUsedList(simulation->memoryModel, memType, out);  
	}
	else if(input == 2)  
	{
	  out = fopen("MEM.TXT", "wt"); 
	  Memory_PrintFreeList(simulation->memoryModel, memType, stdout), 
	  Memory_PrintFreeList(simulation->memoryModel, memType, out);  
	}
	else if(input == 3)
	{
	  out = fopen("MEM.TXT", "wt"); 
	  Memory_PrintMatrix(simulation->memoryModel, memType, stdout),
	  Memory_PrintMatrix(simulation->memoryModel, memType, out);
	}
	else if(input == 4) 
	{
		fclose(out);
		break;
	}

	printf("\nSEE FILE MEM.TXT FOR FULL OUTPUT\n\n");

	fclose(out);

	Simulation_Pause();
  }
}

void Simulation_KillProcess(Simulation *simulation)
{
   Process *process = Simulation_SelectProcess(simulation);
   if(process != NULL) 
   {	  
	   ProcessList_RemoveProcess(simulation->processList, simulation->memoryModel, process->pID);
	   printf("process removed.\n");
   }
   Simulation_Pause();
}

Process *Simulation_SelectProcess(Simulation *simulation)
{
   int pID;
   Process *process;
   int i;

   if(simulation->processList->numProcesses == 0)
   {
	   printf("Process list empty.\n");
	   return NULL;
   }
   
   for(i=0; i<simulation->processList->numProcesses; i++)
	   printf("process %d has pID %d\n", i, simulation->processList->process[i]->pID);

   pID = Simulation_InputInteger("enter the process pid: ", 1);

   process = ProcessList_GetProcess(simulation->processList, pID);

   if(process != NULL) return process;
   else 
   {
	   printf("Process with pID %d not found in main list.\n", pID);
	   return NULL; 
   }
}

void Simulation_SimulateRunning(Simulation *simulation)
{
	Process *process;
	int pageToRequest;
	int numPages;	
	int numPageFaults;
	int choice;
	int i;

	process = Simulation_SelectProcess(simulation);

	if(process != NULL)
	{
		while(1)
		{
			choice = Simulation_DisplaySubMenu(RUN_PROCESS);

			if(choice == 1)
			{// auto mode
				numPages = Simulation_InputInteger("enter the number of references to handle: ", 0);
				numPageFaults = 0;

				for(i=0; i<numPages; i++)
				{					
					Process_FlipReferenceBits(process);
					pageToRequest = rand()%process->numPages;
					printf("num pg %d, to red %d\n", process->numPages, pageToRequest);
					if(ProcessList_RunProcess(simulation->processList, simulation->memoryModel,
						process->pID, pageToRequest)) numPageFaults++;
				}	

				printf("Pages requested: %d\n", numPages);
				printf("Pages faults occured: %d\n", numPageFaults);
				printf("Page Fault Rate: %.2f percent\n", ((float)numPageFaults/(float)numPages)*100.0f);
			}
			else if(choice == 2)
			{
				Process_FlipReferenceBits(process);

				pageToRequest = Simulation_InputProcessPage(process);

				ProcessList_RunProcess(simulation->processList, simulation->memoryModel,
					process->pID, pageToRequest-1);
			}
			else if(choice == 3) break;

			Simulation_Pause();
		}
	}
	else Simulation_Pause();
}

int Simulation_InputInteger(char *query, int restrict)
{
	int input;
	printf("%s", query);
	scanf("%d", &input);
	while(1)
	{
		if(input < 0) printf("please enter a positive integer\n");
		else if(restrict && input > 99) printf("please enter a number with two digits (for simulation purpose).\n");
		else break;
		scanf("%d", &input);
	}
	return input;
}

int Simulation_InputYesNo(char *query)
{
  char input[10];
  printf("%s - yes/no: ", query);
  scanf("%s", input);
  while(1)
  {
	if(strcmp(input, "yes")==0) return 1;
	if(strcmp(input, "no")==0) return 0;
	printf("please enter only 'yes or 'no' keywords\n");
	scanf("%s", input);
  }
}

int Simulation_InputProcessPage(Process *process)
{
	int input;
	printf("enter page to request (1 - %d): ", process->numPages);

	scanf("%d", &input);
	while(input < 1 || input > process->numPages)
	{
		printf("please enter page number in range 1 - %d: ", process->numPages);
		scanf("%d", &input);
	}
	return input;
}

void Simulation_Pause()
{
	fflush(stdin);
	printf("Press return to continue.");
	getc(stdin);
	getc(stdin);
}
