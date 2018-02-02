// HookCopy.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>

#include "HookCopy.h"
#include ".\ShellCodeDetector\mhook-lib\mhook.h"

#include "Inject.h"

WCHAR g_szExistPath[MAX_PATH]={0};
WCHAR g_szNewFilePath[MAX_PATH] = {0};


 //  https://www.cnblogs.com/zeroone/p/3766133.html


#define MAX_LOADSTRING 100
void xLogA(const CHAR *szfmt, ...)
{
    char buff[1024]     = {0};
    va_list	arglist;
    va_start( arglist, szfmt );
    vsprintf_s( (char *)buff, 1024, szfmt, arglist );
    va_end( arglist );

    OutputDebugStringA(buff);
}

typedef BOOL (WINAPI *pHookMoveFileExW)(    _In_     PWSTR lpExistingFileName,
                                        _In_     PWSTR lpNewFileName,
                                        _In_     DWORD dwFlags);

typedef BOOL (WINAPI *pHookCreateDirectoryW)(
    _In_     PWSTR                 lpPathName,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );

pHookMoveFileExW pMoveFileExW = NULL;
pHookCreateDirectoryW  pCreateDirectoryW = NULL;

BOOL WINAPI MyHookMoveFileExW(_In_ PWSTR lpExistingFileName ,
                            _In_opt_ PWSTR lpNewFileName,
                            _In_ DWORD dwFlags)
{
    xLogA("MoveFileExW: %S -> %S\n",lpExistingFileName,lpNewFileName);
    return pMoveFileExW(lpExistingFileName,lpNewFileName,dwFlags);

}


BOOL WINAPI MyHookCreateDirectoryW(
                                   _In_     PWSTR                 lpPathName,
                                   _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes
                                   )
{
    return pCreateDirectoryW(lpPathName,lpSecurityAttributes);
}




int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{


    MessageBoxW(NULL,L"WinMain",L"Tip",MB_OK);

    //×¢ÈëÒ»¸ö dll
    DWORD dwPId  = GetCurrentProcessId();

    InjectDll(dwPId,"APIHook.dll");

    Sleep(2000);
    
    MessageBoxW(NULL,L"WinMain2",L"Tip",MB_OK);

    return 0;

    wcscpy(g_szExistPath,L"C:\\Users\\malihui\\AppData\\Roaming\\Qt\\qtlicenses.ini");
    wcscpy(g_szNewFilePath,L"C:\\Users\\malihui\\AppData\\Roaming\\Qt\\abc\\qtlicenses_new.ini");


    PVOID addr = (PVOID)MyHookMoveFileExW;
    PVOID addr2 = (PVOID)GetProcAddress(LoadLibraryA("kernel32.dll"),"MoveFileExW");
    if(Mhook_SetHook(&addr2,addr,0))
    {
        xLogA("Hook MoveFileExW!\n");
        pMoveFileExW = (pHookMoveFileExW)addr2;
    }

    getchar();
	return 0;
}


