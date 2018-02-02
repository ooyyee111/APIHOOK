// test0131.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#pragma comment(lib, "Psapi.lib").


void PrintModules( DWORD processID )
{
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;

    // Print the process identifier.

    printf( "\nProcess ID: %u\n", processID );

    // Get a list of all the modules in this process.

    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, processID );
    if (NULL == hProcess)
        return;

    if( EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
        {
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module's file.

            if ( GetModuleFileNameEx(hProcess, hMods[i], szModName,
                sizeof(szModName)/sizeof(TCHAR)))
            {
                // Print the module name and handle value.

                _tprintf(TEXT("\t%s (0x%08X)\n"),
                    szModName, hMods[i]);
            }
        }
    }

    CloseHandle( hProcess );
}

void main( )
{
    // Get the list of process identifiers.
    CHAR    g_szProcName[MAX_PATH]                      = {};

    GetModuleFileNameExA(GetCurrentProcess(), NULL, g_szProcName, MAX_PATH);

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
        return ;

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name of the modules for each process.

    for ( i = 0; i < cProcesses; i++ )
        PrintModules( aProcesses[i] );

    return ;

}

