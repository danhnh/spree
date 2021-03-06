#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <tchar.h>

#define FILE_LOG_BUF_SIZE	1024

#ifdef FILELOG_EXPORTS
class __declspec(dllexport) CFileLog
#else
class __declspec(dllimport) CFileLog
#endif
{
public:
	CFileLog();
	~CFileLog();

public:
	BOOL CreateFile();
	void CloseFile();

	void Log(LPCTSTR szLog);

private:
	FILE* _m_hFile;
	TCHAR* _m_pBuf;
};

#ifdef FILELOG_EXPORTS
__declspec(dllexport) void LOGOUT(LPCTSTR pszFormat, ...);
__declspec(dllexport) void LOGOUT2(LPCTSTR pszLog);
#else
__declspec(dllimport) void LOGOUT(LPCTSTR pszFormat, ...);
__declspec(dllimport) void LOGOUT2(LPCTSTR pszLog);
#endif

#ifndef FILELOG_EXPORTS
	#ifndef _FILE_LOG_LIB_
	#define _FILE_LOG_LIB_
		#ifdef _UNICODE
			#pragma comment(lib, "uFileLog.lib")
		#else
			#pragma comment(lib, "mFileLog.lib")
		#endif
	#endif
#endif
