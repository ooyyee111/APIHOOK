#pragma once
typedef DWORD (WINAPI *PFNTCREATETHREADEX)
( 
 PHANDLE                 ThreadHandle,    
 ACCESS_MASK             DesiredAccess,    
 LPVOID                  ObjectAttributes,    
 HANDLE                  ProcessHandle,    
 LPTHREAD_START_ROUTINE  lpStartAddress,    
 LPVOID                  lpParameter,    
 BOOL                    CreateSuspended,    
 DWORD                   dwStackSize,    
 DWORD                   dw1, 
 DWORD                   dw2, 
 LPVOID                  Unknown 
 );

BOOL InjectDll(DWORD dwPID, char *szDllName);