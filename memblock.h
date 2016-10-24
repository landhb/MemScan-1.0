// Memscan.h
// Written by: Bradley Landherr
// Purpose: Includes and Struct definitions for memscan.c

#include <windows.h>
#include <stdio.h>

// Memory flags we're interested pertaining to readability, writability, and execute 
#define WRITABLE (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)

// Macros

// IS_IN_SEARCH allows us to query an individual flag inside a byte of memory
#define IS_IN_SEARCH(mb, offset) (mb->searchmask[(offset)/8] & (1<<((offset)%8)))

// Ex. Find flag for byte 20 in the buffer
// will search byte 2 of the searchmask (mb->searchmask[20/8] = mb->searchmask[2])
// 20%8 = 4, So IS_IN_SEARCH will search bit 4 of byte 2

#define REMOVE_FROM_SEARCH(mb, offset) (mb->searchmask[(offset)/8] &= ~(1<<((offset)%8)))

/* REFERENCE for MEMORY_BASIC_INFORMATION

https://msdn.microsoft.com/en-us/library/windows/desktop/aa366775(v=vs.85).aspx

typedef struct _MEMORY_BASIC_INFORMATION {
  PVOID  BaseAddress;
  PVOID  AllocationBase;
  DWORD  AllocationProtect;
  SIZE_T RegionSize;
  DWORD  State;
  DWORD  Protect;
  DWORD  Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;


*/

// Define a data structure for our memory blocks
typedef struct _MEMBLOCK {

	// Handle of process we're interested in 
	HANDLE hProc;

	// Pointer for the base address of this process' memblock
	unsigned char *addr;

	// Size of memblock
	int size;

	// Local buffer to copy data into for reading/writing
	unsigned char *buffer;

	// Search flags stored in corresponding buffer
	unsigned char *searchmask;
	int matches;				// Number of matches still in buff
	int data_size;				// Search size (byte, DWORD, ect.)

	// Pointer to the next memory block in the series (arranging as a linked-list)
	struct _MEMBLOCK *next;

} MEMBLOCK;

// Search conditions
typedef enum {

	COND_UNCONDITIONAL, 	// Consider every byte
	COND_EQUALS,			// Only include a byte if it matches a particular value

	COND_INCREASE,
	COND_DECREASE,

} SEARCH_CONDITION;


// Read functions

void read_memblock (MEMBLOCK *mb, SEARCH_CONDITION condition, unsigned int val);
void read_scan (MEMBLOCK *mb_list, SEARCH_CONDITION condition, unsigned int val);
