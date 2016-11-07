#include <windows.h>
#include <stdio.h>

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
const char * GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = GetLastError();
    if(errorMessageID == 0) {
        return NULL; //No error message has been recorded
    }

    char * messageBuffer = NULL;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);



    return messageBuffer;
}

void Usage() {
    printf(
        "\nUsage for Memscan.exe:\n"
        "\nmemscan.exe [Executable Name] [Search String]\n\n"
        "Options are:\n"
        "    [Executable Name]: Name of the exe that owns the process you wish to examine. (i.e. chrome.exe)\n"
        "      [Search String]: The search term you're looking for in the process\n"
        "               --help: display what you are reading now\n");
}