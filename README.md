# MemScan
## Scan/manipulate the memory of a process with a given PID

#### Overview
1. Discover memory regions in use by a process with [VirtualQueryEx](https://msdn.microsoft.com/en-us/library/windows/desktop/aa366907(v=vs.85).aspx) given a PID
2. Read memory into a local structure with [ReadProcessMemory](https://msdn.microsoft.com/en-us/library/windows/desktop/ms680553(v=vs.85).aspx)
3. Modify the content of the memory locally
4. Write the modified memory back into the process with [WriteProcessMemory](https://msdn.microsoft.com/en-us/library/windows/desktop/ms681674(v=vs.85).aspx)

![Unique String](https://github.com/landhb/MemScan-1.0/blob/master/img/example.PNG?raw=true "Scan unique string")

#### Compiling w/Linux Subsystem

To create Windows executables in the linux subsystem, you need to install mingw cross-compiler:

```
sudo apt-get install mingw-w64
```

Then you can create 32-bit Windows executables using the makefile with:

```
make 32bit
```

And 64-bit Windows executables with:

```
make 64bit
```

#### Usage

Program takes the name of the exe (i.e. "slack.exe") running the process you'd like to examine and the search string.

```
.\memscan.exe [Process Name] [Search String]
```

**Note: memscan.exe must be compiled as a 64bit executable to examine 64 bit processes** 
