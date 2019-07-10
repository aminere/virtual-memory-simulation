#ifndef __simulation_h_
#define __simulation_h_

#include <stdlib.h>
#include "processlist.h"
#include "_memory_.h"

#define CLEAR_COMMAND "cls"		// windows version
//#define CLEAR_COMMAND "clear" // linux version

typedef enum _SUB_MENU
{
  DISPLAY_MEMORY_STATE=0, RUN_PROCESS
}SUB_MENU;

typedef struct _Simulation
{

  ProcessList *processList;
  MemoryModel *memoryModel;

}Simulation;

// SIMULATION FUNCS

Simulation *Simulation_Create();

void Simulation_Run(Simulation *simulation);

void Simulation_CreateNewProcess(Simulation *simulation);

int Simulation_DisplayMenu();

int Simulation_DisplaySubMenu(SUB_MENU submenu);

void Simulation_DisplayMemoryState(Simulation *simulation, char *memType);

void Simulation_DisplayProcessResidentSet(Simulation *simulation);

void Simulation_DisplayProcessPageTable(Simulation *simulation);

void Simulation_KillProcess(Simulation *simulation);

Process *Simulation_SelectProcess(Simulation *simulation);

void Simulation_SimulateRunning(Simulation *simulation);

int Simulation_InputInteger(char *query, int restrict);

int Simulation_InputYesNo(char *query);

void Simulation_Pause();

int Simulation_InputProcessPage(Process *process);

#endif