// Forced into only the two Windows backend translation units by run.ps1.
// Read the Win32 declarations before replacing the call sites.
# pragma once
# include <Siv3D/Windows/Windows.hpp>

LPVOID WINAPI MappingCheckMapViewOfFile(HANDLE mapping, DWORD access,
	DWORD offsetHigh, DWORD offsetLow, SIZE_T bytes);
BOOL WINAPI MappingCheckCloseHandle(HANDLE handle);

# define MapViewOfFile MappingCheckMapViewOfFile
# define CloseHandle MappingCheckCloseHandle
