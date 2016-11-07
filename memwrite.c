// memwrite.c
// Written by: Bradley Landherr
// Purpose: Use WriteProcessMemory to re-write memory at a given memory location

#include "memblock.h"

void poke (HANDLE hProc, int data_size, unsigned int * addr, unsigned int val) {
	if (WriteProcessMemory (hProc, (LPVOID)addr, &val, data_size, NULL) == 0) {
		printf("%s\n", "Poke Failed");
	}
}

// Technically a Read function, but I put it here
unsigned int peek (HANDLE hProc, int data_size, unsigned char * addr) {
	unsigned int val = 0;

	if (ReadProcessMemory (hProc, addr, &val, data_size, NULL) == 0) {
		printf("Peek failed\r\n");
	}

	return val;
}