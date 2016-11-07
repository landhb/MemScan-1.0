// memread.c
// Written by: Bradley Landherr
// Purpose: Use ReadProcessMemory to read memory into a buffer mb->buffer


#include "memblock.h"

int _search_memblock (MEMBLOCK *mb, SEARCH_CONDITION condition, unsigned int val, unsigned char* tempbuf, SIZE_T bytes_read, unsigned int total_read) {

	// Conduct the search
	if (condition == COND_UNCONDITIONAL) {
		// Search all
		memset (mb->searchmask + (total_read/8), 0xff, bytes_read/8);
		mb->matches += bytes_read;
	}
	else {
		unsigned int offset;

		for (offset = 0; offset < bytes_read;  offset += mb->data_size) {
			// Check if byte is still in search, conduct search
			if (IS_IN_SEARCH(mb, (total_read + offset))) {
				BOOL is_match = FALSE;
				unsigned int temp_val;

				switch (mb->data_size) {
					// Case of 1 byte data size
					case 1:
						// temp_val is just the bit at the offset in the byte
						temp_val = tempbuf[offset];
						break;
					case 2:
						temp_val = *((unsigned short*) &tempbuf[offset]);
						break;
					case 4:
					default:
						temp_val = *((unsigned int *)&tempbuf[offset]);
						break;
				}

				switch (condition) {
					case COND_EQUALS:
						is_match = (temp_val == val);
						break;
					default:
						break;
				}

				// If match, increment counter, else remove from search bitmap
				if (is_match) {
					mb->matches++;
				}
				else {
					REMOVE_FROM_SEARCH (mb, (total_read + offset));
				}
			}
		}
	}

}

// Function: Read the contents of a memblock using ReadProcessMemory
// Returns: Buffer of read memory in mb->buffer
void read_memblock (MEMBLOCK *mb, SEARCH_CONDITION condition, unsigned int val) {

	// local buffer
	static unsigned char tempbuf[128*1024];

	// read variables
	unsigned int bytes_left;		// How many bytes left
	unsigned int total_read; 		// Total already read
	SIZE_T bytes_to_read;			// Bytes to read on current iteration
	SIZE_T bytes_read;				// Bytes read after ReadProcessMemory iteration

	// Begin with all the data left, 0 read, and 0 current matches
	bytes_left = mb->size;
	total_read = 0;
	mb->matches = 0;

	while(bytes_left) {

		// If bytes_left is greater than sizeof(tempbuf)  only read in tempbuf bytes, otherwise just read bytes_left
		bytes_to_read = (bytes_left > sizeof(tempbuf)) ? sizeof(tempbuf) : bytes_left;

		// ReadProcessMemory(_In_ HANDLE hProcess,_In_ LPCVOID lpBaseAddress,_Out_ LPVOID lpBuffer,_In_ SIZE_T nSize,_Out_ SIZE_T *lpNumberOfBytesRead);
		ReadProcessMemory (mb->hProc, mb->addr + total_read, &tempbuf, bytes_to_read, &bytes_read);

		// If the number of bytes_read is not what we expected, exit
		if (bytes_read != bytes_to_read) {
			break;
		}
		
		// Search memblock 
		_search_memblock (mb, condition, val, tempbuf, bytes_read, total_read);

		// Copy tempbuf onto the mb->buffer
		memcpy(mb->buffer + total_read, tempbuf, bytes_read);

		// Update values
		bytes_left -= bytes_read;
		total_read += bytes_read;
	}

	// Ensure size is accurate after ReadProcessMemory
	mb->size = total_read;
}

// Loop through linked list reading memory for each block
void read_scan (MEMBLOCK *mb_list, SEARCH_CONDITION condition, unsigned int val){

	MEMBLOCK *mb = mb_list;

	while (mb) {
		read_memblock(mb, condition, val);
		mb = mb->next;
	}
}

void print_matches (MEMBLOCK *mb_list) {
	unsigned int offset;

	MEMBLOCK *mb = mb_list;

	while(mb) {
		for (offset = 0; offset < mb->size; offset += mb->data_size) {
			if (IS_IN_SEARCH(mb,offset)) {
				unsigned int val = peek(mb->hProc, mb->data_size, mb->addr + offset);
				printf("%0x08x: 0x%08x (%d) \r\n", mb->addr + offset, val, val);
			}
		}
		mb = mb->next;
	}
}

int get_match_count (MEMBLOCK *mb_list) {
	MEMBLOCK *mb = mb_list;

	int count = 0;

	while (mb) {
		count += mb->matches;
		mb = mb->next;
	}
	printf("%s %d\n", "Total matches found: ", count);
	return count;
}