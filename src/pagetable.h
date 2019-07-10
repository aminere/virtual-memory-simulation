#ifndef __pagetable_h_
#define __pagetable_h_

typedef struct _PageTableEntry
{ // Represents an entry in a process page table

  int P; // represents the P (present) bit 

  int R; // represents the R (referenced) bit
  
  int frame; // frame index in Real Memory 
			 // consistent only if P = 1

}PageTableEntry;

#endif;