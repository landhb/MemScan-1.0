// Memscan.c
// Written by: Bradley Landherr
// Purpose: Scan memory of a process given a PID

#include <windows.h>
#include <stdio.h>

// Memory flags we're interested pertaining to readability, writability, and execute 
#define WRITABLE (PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)

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

	// Pointer to the next memory block in the series (arranging as a linked-list)
	struct _MEMBLOCK *next;

} MEMBLOCK;


// Function: Create a memblock
// Returns: pointer to a memory block
// VirtualQueryEx returns the MEMORY_BASIC_INFORMATION structure
MEMBLOCK* create_memblock (HANDLE hProc, MEMORY_BASIC_INFORMATION *meminfo) {
	
	// Allocate memory for the block
	MEMBLOCK *mb = malloc (sizeof(MEMBLOCK));

	// If successfully allocated initialize the memblock
	if (mb) {
		mb->hProc = hProc;
		mb->addr = meminfo->BaseAddress;
		mb->size = meminfo->RegionSize;
		mb->buffer = malloc(meminfo->RegionSize);
		mb->next = NULL;
	}

	return mb;
}


// Function: Free a memblock from memory
// Returns: void
void free_memblock (MEMBLOCK *mb) {

	// Check pointer
	if (mb) {
		if (mb->buffer) {
			free (mb->buffer);
		}

		free (mb);
	}
}

// Function: Create scan
// Returns: Pointer to memblock that is the head of a complete linked list of the memoryblocks of a process
MEMBLOCK* create_scan (unsigned int pid) {

	// Initialize the pointer to the head of our linked list
	MEMBLOCK *mb_list = NULL;

	// Variable for result of VirtualQueryEx
	MEMORY_BASIC_INFORMATION meminfo;

	// Address where VirtualQueryEx should begin searching (initially zero before head process is found)
	unsigned char *addr = 0;

	// Handle to the process we're interested in (need PROCESS_ALL_ACCESS to manipulate the process' memory)
	HANDLE hProc = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pid);

	// If we recieve a valid process handle
	if (hProc) {

		// Loop through process memory
		while (1) {

			// VirtualQueryEx takes (handle of process, address to start looking from, result variable, sizeof(result))
			// Returns non-zero if successful
			if (VirtualQueryEx (hProc, addr, &meminfo, sizeof(meminfo)) == 0) {
				// If VirtualQueryEx returns zero, exit due to failure (or end of memory for process)
				break;
			}

			// Filter out uncommitted/unused memory and memory that isn't writable
			if ((meminfo.State & MEM_COMMIT) && (meminfo.Protect & WRITABLE)) {


				// Create a local instance of the memory found through VirtualQueryEx
				MEMBLOCK *mb = create_memblock (hProc, &meminfo);

				// If valid, add to the head of our linked-list
				if (mb) {

					// Next pointer to current head of linked-list
					mb->next = mb_list;

					// Change head to our new memory block
					mb_list = mb;
				}
			}

			// Update the next addr to begin search
			addr = (unsigned char*)meminfo.BaseAddress + meminfo.RegionSize;
		}
	}
	return mb_list;
}

void free_scan (MEMBLOCK *mb_list) {

	// Close the process handle we opened
	CloseHandle (mb_list->hProc);

	// Iterate through the linked list freeing memory blocks
	while (mb_list) {

		// Get head of the list
		MEMBLOCK *mb = mb_list;

		// Move start of the list to the next block
		mb_list = mb_list->next;

		// Free the block we pulled from the head of the list
		free_memblock (mb);
	}
}

void dump_scan_info (MEMBLOCK *mb_list) {

	// Pointer to head of the linked-list
	MEMBLOCK *mb = mb_list;

	// Loop through the list
	while (mb) {
		printf("0x%08x %d\r\n", mb->addr, mb->size);
		mb = mb->next;
	}

}

int main(int argc, char *argv[]) {

	// Create a scan given a PID
	MEMBLOCK *scan = create_scan (atoi(argv[1]));

	// If created dump info and free the memory
	if (scan) {
		dump_scan_info(scan);
		free_scan(scan);
	}
	return 0;
}