# MemScan
## Scan/manipulate the memory of a process with a given PID

#### Overview
1. Discover memory regions in use by a process with [VirtualQueryEx](https://msdn.microsoft.com/en-us/library/windows/desktop/aa366907(v=vs.85).aspx) given a PID
2. Read memory into a local structure with [ReadProcessMemory](https://msdn.microsoft.com/en-us/library/windows/desktop/ms680553(v=vs.85).aspx)
3. Modify the content of the memory locally
4. Write the modified memory back into the process with [WriteProcessMemory](https://msdn.microsoft.com/en-us/library/windows/desktop/ms681674(v=vs.85).aspx)


#### Compiling w/Linux Subsystem

To create Windows executables in the linux subsystem, you need to install mingw cross-compiler:

```
sudo apt-get install mingw-w64
```

Then you can create 32-bit Windows executable with:

```
i686-w64-mingw32-gcc -o memscan.exe memscan.c memread.c memblock.h
```

And 64-bit Windows executable with:

```
x86_64-w64-mingw32-gcc -o memscan.exe memscan.c memread.c memblock.h
```

#### Usage

```
.\memscan.exe [PID]
```