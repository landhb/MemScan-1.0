// memwrite.c
// Written by: Bradley Landherr
// Purpose: Use WriteProcessMemory to re-write memory at a given memory location

#include "memblock.h"

void poke (HANDLE hProc, int data_size, unsigned int * addr, unsigned char * val) {
	if (WriteProcessMemory (hProc, (LPVOID)addr, &val, data_size, NULL) == 0) {
		printf("%s\n", "Poke Failed");
	}
}

// Technically a Read function, but I put it here
unsigned char * peek (HANDLE hProc, int data_size, unsigned char * addr, int size) {
	unsigned char * val;
	//unsigned char val[size];

	if (ReadProcessMemory (hProc, addr, &val, size, NULL) == 0) {
		printf("Peek failed\r\n");
	}

	return val;
}