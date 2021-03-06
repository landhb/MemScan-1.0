// Memscan.c
// Written by: Bradley Landherr
// Purpose: Scan memory of a process given a PID

#include "memblock.h"


// Function: Create a memblock
// Returns: pointer to a memory block
// VirtualQueryEx returns the MEMORY_BASIC_INFORMATION structure
MEMBLOCK* create_memblock (HANDLE hProc, MEMORY_BASIC_INFORMATION *meminfo, int data_size) {
	
	// Allocate memory for the block
	MEMBLOCK *mb = malloc (sizeof(MEMBLOCK));

	// If successfully allocated initialize the memblock
	if (mb) {
		mb->hProc = hProc;
		mb->addr = meminfo->BaseAddress;
		mb->size = meminfo->RegionSize;
		mb->buffer = malloc(meminfo->RegionSize);
		mb->searchmask = malloc(meminfo->RegionSize/8);
		memset (mb->searchmask, 0xff, meminfo->RegionSize/8);
		mb->matches = meminfo->RegionSize;
		mb->data_size = data_size;
		mb->next = NULL;
	} else {
		printf("%s\n", "Error: Could not allocate memory for memblock");
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
		if (mb->searchmask) {
			free (mb->searchmask);
		}

		free (mb);
	}
}



// Function: Create scan
// Returns: Pointer to memblock that is the head of a complete linked list of the memoryblocks of a process
MEMBLOCK* create_scan (unsigned int pid, int data_size) {

	// Initialize the pointer to the head of our linked list
	MEMBLOCK *mb_list = NULL;

	// Variable for result of VirtualQueryEx
	MEMORY_BASIC_INFORMATION meminfo;

	// Address where VirtualQueryEx should begin searching (initially zero before head process is found)
	unsigned char *addr = 0;

	// Handle to the process we're interested in (need PROCESS_ALL_ACCESS to manipulate the process' memory)
	HANDLE hProc = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pid);

	// SYSTEM INFO to check bounds of VirtualQueryEx
	SYSTEM_INFO si;
	GetSystemInfo(&si);


	// If we recieve a valid process handle
	if (hProc) {

		// Loop through process memory
		while (addr < (unsigned char *)si.lpMaximumApplicationAddress) {

			// VirtualQueryEx takes (handle of process, address to start looking from, result variable, sizeof(result))
			// Returns non-zero if successful
			if (VirtualQueryEx (hProc, addr, &meminfo, sizeof(meminfo)) == 0) {
				// If VirtualQueryEx returns zero, exit due to failure (or end of memory for process)
				printf("%s %s\n", "VirtualQueryEx Error: ", GetLastErrorAsString());
				break;
			}

			// Filter out uncommitted/unused memory and memory that isn't writable
			if ((meminfo.State & MEM_COMMIT) && (meminfo.Protect & WRITABLE)) {


				// Create a local instance of the memory found through VirtualQueryEx
				MEMBLOCK *mb = create_memblock (hProc, &meminfo, meminfo.RegionSize);

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
	} else {
		printf("%s %d %s\n", "Exiting: Process ", pid, " not found.");
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
		int i;
		printf("0x%08x %d\r\n", mb->addr, mb->size);

		for (i = 0; i < mb->size; i++) {
			//printf("%02x", mb->buffer[i]);

			printf("%s", &mb->buffer[i]);
		}
		printf("\r\n");
		mb = mb->next;
	}

}


int main(int argc, char *argv[]) {


	// Usage
	if ( argc != 3) {
		Usage();
	}


	// Get the PID of the given process
	unsigned int pid = FindProcessId(argv[1]);

	if (pid == 0) {
		printf("Failed to find process %s\n", argv[1]);
		return 2;
	}

	// Get size of search string
	int size = 0;
	while(argv[2][size] != '\0') {

		size++;
	}

	MEMBLOCK * scan = create_scan (pid, size); 

	// If created dump info and free the memory

	if (scan) {
		printf("%s\n", "Created scan");
		read_scan(scan, COND_EQUALS, argv[2], size);
		//dump_scan_info(scan);
		print_matches(scan, size);
		get_match_count(scan);
		free_scan(scan);
	}else {
		printf("%s\n", "Failed to create scan...");
	}

	return 0;
}