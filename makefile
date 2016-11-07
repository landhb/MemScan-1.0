memscan: memscan.c memread.c memwrite.c tools/errorhandling.c tools/process.c
	i686-w64-mingw32-gcc -o memscan.exe memscan.c memread.c memwrite.c tools/errorhandling.c tools/process.c memblock.h