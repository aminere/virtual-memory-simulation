// CSC3351 - Operating Systems
// Paging / Virtual Memory Simulation
// By Amine Rehioui 
// and Med Neyl Benjelloun.

#include "simulation.h"

int main(int argc, char *argv[])
{
  Simulation *simulation = Simulation_Create();

  Simulation_Run(simulation);

  return 0;
}