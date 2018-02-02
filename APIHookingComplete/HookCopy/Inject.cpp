#include "StdAfx.h"
#include "stdio.h"
#include "tchar.h"
#include "Inject.h"
#include <Windows.h>

BOOL IsVistaOrLater()
{
    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&osvi);
    if( osvi.dwMajorVersion >= 6 )
        return TRUE;
    return FALSE;
}

BOOL MyCreateRemoteThread(HANDLE hProcess, LPTHREAD_START_ROUTINE pThreadProc, LPVOID pRemoteBuf)
{
    HANDLE      hThread = NULL;
    FARPROC     pFunc = NULL;
    if( IsVistaOrLater() )    // Vista, 7, Server2008
    {
        pFunc = GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtCreateThreadEx");
        if( pFunc == NULL )
        {
            return FALSE;
        }
        ((PFNTCREATETHREADEX)pFunc)(&hThread,
            0x1FFFFF,
            NULL,
            hProcess,
            pThreadProc,
            pRemoteBuf,
            FALSE,
            NULL,
            NULL,
            NULL,
            NULL);
        if( hThread == NULL )
        {
            //printf("MyCreateRemoteThread() : NtCreateThreadEx() 调用失败！错误代码: [%d]/n", GetLastError());
            return FALSE;
        }
    }
    else                    // 2000, XP, Server2003
    {
        hThread = CreateRemoteThread(hProcess, 
            NULL, 
            0, 
            pThreadProc, 
            pRemoteBuf, 
            0, 
            NULL);
        if( hThread == NULL )
        {
            //printf("MyCreateRemoteThread() : CreateRemoteThread() 调用失败！错误代码: [%d]/n", GetLastError());
            return FALSE;
        }
    }
    if( WAIT_FAILED == WaitForSingleObject(hThread, INFINITE) )
    {
        //printf("MyCreateRemoteThread() : WaitForSingleObject() 调用失败！错误代码: [%d]/n", GetLastError());
        return FALSE;
    }
    return TRUE;
}

BOOL InjectDll(DWORD dwPID, char *szDllName)
{
    HANDLE hProcess = NULL;
    LPVOID pRemoteBuf = NULL;
    FARPROC pThreadProc = NULL;
    char szAppPath[MAX_PATH]; 
    GetModuleFileNameA(NULL, szAppPath, MAX_PATH);
    (strrchr(szAppPath, '\\'))[1] = 0;
    strcat(szAppPath,szDllName);
    DWORD dwBufSize = strlen(szAppPath)+1;
    if ( !(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)) )
    {
        //printf("[错误] OpenProcess(%d) 调用失败！错误代码: [%d]/n", dwPID, GetLastError());
        return FALSE;
    }
    pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, 
        MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szAppPath, 
        dwBufSize, NULL);
    pThreadProc = GetProcAddress(GetModuleHandleA("kernel32.dll"),"LoadLibraryA");

    if( !MyCreateRemoteThread(hProcess, (LPTHREAD_START_ROUTINE)pThreadProc, pRemoteBuf) )
    {
        //printf("[错误] CreateRemoteThread() 调用失败！错误代码: [%d]/n", GetLastError());
        return FALSE;
    }

    VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
    CloseHandle(hProcess);
    return TRUE;
}