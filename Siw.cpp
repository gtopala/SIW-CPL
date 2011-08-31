// $Id: Siw.cpp,v 1.2 2011/08/10 13:24:55 gabriel Exp $
// Date: July 07,
// Author: JT
// Control Panel app (DLL) as interface for asscociated app
// Associated App: Siw.exe
// Dialog of associated app is used thus no dialog defs in dll

// Siw.p : Defines the entry point for the DLL application.
//
// Project Settings
//		C/C++ Tab, Preprocessor definitions: WIN32,NDEBUG,_WINDOWS,_MBCS,_USRDLL,_WINDLL,
//      Link Tab.  Project Options: extra param: /def:"SIW.def"
#include "Siw.h"

APPLET SiwApplet = {	IDI_APP,			// Icon
						IDS_PANEL_NAME,		// "Siw", 
						IDS_PANEL_DESC,		// "System Info for Windows", 
						0,					// IDD_MAIN removed from resource, 
						NULL,			    // DlgWinKeys,
					};


BOOL WINAPI DllMain( HINSTANCE	hinstDLL,
                     DWORD		ulReason_for_call, 
                     LPVOID		lpvReserved)
{
	if (ulReason_for_call != DLL_PROCESS_ATTACH)
    {
		return TRUE;
    }
    else
    {
        hInstance = hinstDLL;
    }

    return TRUE;
}



LONG CALLBACK CPlApplet (HWND hwndCPL, UINT uMsg, LONG lParam1, LONG lParam2)
{    
	LPNEWCPLINFO lpNewCPlInfo;
 
    switch (uMsg) 
	{ 
        case CPL_INIT:      // first message, sent once 
			return TRUE; 
 
        case CPL_GETCOUNT:  // second message, sent once 
            return (LONG) NUM_APPLETS; 
            break; 
 
        case CPL_NEWINQUIRE: // third message, sent once per application 
            lpNewCPlInfo = (LPNEWCPLINFO) lParam2; 
	
			lpNewCPlInfo->dwSize = (DWORD) sizeof(NEWCPLINFO);
            lpNewCPlInfo->dwFlags= 0;
            lpNewCPlInfo->lData	 = 0;
			lpNewCPlInfo->dwHelpContext = 0;
			lpNewCPlInfo->hIcon = LoadIcon(hInstance, (LPCTSTR) MAKEINTRESOURCE(SiwApplet.icon));
			lpNewCPlInfo->szHelpFile[0] = '\0';
			LoadString (hInstance, SiwApplet.namestring, lpNewCPlInfo->szName, 32);
            LoadString (hInstance, SiwApplet.descstring, lpNewCPlInfo->szInfo, 64);
            break; 

        case CPL_DBLCLK:    // application icon double-clicked 
		{
            HWND	hwndSiw = NULL;
			//TCHAR	szSiw[MAX_PATH];
			//TCHAR	szSiwPath[MAX_PATH]= {""};
			//TCHAR	szSiwTitle[MAX_COMPUTERNAME_LENGTH + 55];
			//TCHAR	szComputerName[MAX_COMPUTERNAME_LENGTH + 1];

            TCHAR	szSiw[MAX_PATH+16] = { 0 };
			TCHAR	szSiwPath[MAX_PATH+16] = { 0 };
			TCHAR	szSiwTitle[MAX_COMPUTERNAME_LENGTH + 64] = { 0 };
			TCHAR	szComputerName[MAX_COMPUTERNAME_LENGTH + 16] = { 0 };
            DWORD	dwSize = MAX_COMPUTERNAME_LENGTH + 1;

			GetComputerName(szComputerName, &dwSize);
			wsprintf(szSiwTitle, "%s%s", "System Info  © Gabriel Topala- running on \\\\", szComputerName);
			
			if ((hwndSiw = FindWindow((LPCTSTR) NULL, (LPCTSTR) szSiwTitle)) != NULL)
			{
				ShowWindow(hwndSiw, SW_SHOW);
				SetFocus(hwndSiw);
				SetForegroundWindow(hwndSiw);
				break;
			}
			
			strcpy_s(szSiwPath, MAX_PATH, GetRegKeyApp());
			if (strlen(szSiwPath) == 0)  // Show Dialog to get Path
			{
				strcpy_s(szSiwPath, MAX_PATH, BrowseFolderName(GetActiveWindow()));
				WriteRegKeys(szSiwPath);
			}	
				
            wsprintf(szSiw, "\"%s%s\"", szSiwPath, "\\Siw.exe");
			if (WinExec(szSiw, SW_SHOW) < 31)
			{
				if (::ShellExecute(/*hwnd, */NULL, "open", szSiw, NULL, NULL, SW_SHOWNORMAL) < reinterpret_cast<HINSTANCE>(32))
					MessageBox(GetActiveWindow(), "Error starting Siw.exe", "Error", MB_ICONERROR);
			}

			//DialogBox( (HINSTANCE) hModule, 
			//			MAKEINTRESOURCE(SiwApplet.dlgtempl), 
			//			hwndCPL, SiwApplet.dlgfn); 
            break; 
		}

        case CPL_STOP:      // sent once per application before CPL_EXIT 
			break; 
 
        case CPL_EXIT:		// sent once before FreeLibrary is called 
			break; 
 
        default: 
            break; 
    } 

    return 0; 
} 


LPCTSTR GetRegKeyApp()  // Get Path value
{
	LONG	lRes = 0;
	HKEY	hKey = HKEY_LOCAL_MACHINE;
	LPCTSTR lpSubKey = "Software\\NKY Inc.\\Siw";
	DWORD	dwOptions = REG_OPTION_NON_VOLATILE;
	HKEY	hkResult = 0;  
	TCHAR	szValueName[MAX_PATH]= {"AppPath"};
	DWORD   dwType = REG_SZ;
	DWORD	dwSize;
	TCHAR	szData[MAX_PATH];
	static  TCHAR	szPath[MAX_PATH] = {""};

	lRes = RegOpenKeyEx(hKey, lpSubKey, dwOptions, KEY_ALL_ACCESS, &hkResult);
	if (lRes != ERROR_SUCCESS)
	{
		return (szPath);
	}

	dwSize = sizeof(szData);
	lRes = RegQueryValueEx(hkResult, szValueName, 0L, &dwType, (LPBYTE) szData, &dwSize);
	lstrcpy(szPath, szData);
	RegCloseKey(hkResult);
	RegCloseKey(hKey);
	return (szPath);
}


LPSTR BrowseFolderName (HWND hwnd)
{
	LONG		    lRet;
	static LPSTR    szFolderPath[MAX_PATH]= {""};
    
    bi.ulFlags		= BIF_RETURNONLYFSDIRS;
    bi.hwndOwner	= hwnd;
	bi.lpszTitle	= (LPCSTR) "Select the Location of SIW:";
	bi.pidlRoot		= 0L;

	lRet = SHGetPathFromIDList(SHBrowseForFolder(&bi), (LPSTR) szFolderPath);
	return (LPSTR) szFolderPath;
}


BOOL WriteRegKeys(LPCTSTR szPath) 
{
	LONG	lRes = 0;
	HKEY	hKey = HKEY_LOCAL_MACHINE;
	LPCTSTR lpSubKey = "Software\\NKY Inc.\\Siw";
	DWORD	dwOptions = REG_OPTION_NON_VOLATILE;
	REGSAM	samDesired = KEY_ALL_ACCESS;
	HKEY	hkResult = 0;  
	DWORD	dwDisposition = 0;
	DWORD	dwType = REG_SZ;

	// Set Path value
	lRes = RegOpenKeyEx(hKey, lpSubKey, 0L, samDesired, &hkResult);
	if (! lRes == ERROR_SUCCESS)	// Create Key
	{
		lRes = RegCreateKeyEx(hKey, lpSubKey, 0L, NULL, dwOptions, samDesired, NULL, &hkResult, &dwDisposition);
		if (! lRes == ERROR_SUCCESS) // Create Key failed
		{	
			return (FALSE);
		}
		lRes = RegOpenKeyEx(hKey, lpSubKey, 0L, samDesired, &hkResult);
	}

	lRes = RegSetValueEx(hkResult, (LPCTSTR) "AppPath", 0L, dwType,(CONST BYTE *) szPath, (DWORD) (lstrlen(szPath) +1));
	
	if (! lRes == ERROR_SUCCESS) 
	{
		MessageBox(GetActiveWindow(), "Error setting registry values", "", MB_OK | MB_ICONERROR);
		RegCloseKey(hkResult);
		RegCloseKey(hKey);
		return (FALSE);
	}

	RegCloseKey(hkResult);
	RegCloseKey(hKey);
	return (TRUE);
}



/*

BOOL OSIsVersionNT4()
{
	// Return true on NT version > 4
	DWORD	dwWindowsMajorVersion, dwWindowsMinorVersion;
	DWORD	dwVersion = GetVersion();
 
	dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
	dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
	
	if (dwWindowsMajorVersion < 4) return (FALSE);

	if (dwWindowsMajorVersion == 4) {
		if ((dwWindowsMinorVersion == 1) || // Windows 9x, or Windows Me
			(dwWindowsMinorVersion == 10))
			return (FALSE);
		return (TRUE);				// NT 4
	}
									// Windows 2000 or XP have value 5
	return (TRUE);
}


void DeleteRegKeys() 
{
	LONG	lRes = 0;
	HKEY	hKey = HKEY_LOCAL_MACHINE;
	HKEY	hkResult;
	TCHAR	lpSubKey[256] = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	LPCTSTR lpValueName;
	TCHAR	szSubKey[MAX_PATH] = {""};
	
	lRes = RegOpenKeyEx(hKey, lpSubKey, 0, KEY_ALL_ACCESS, &hkResult);
	if (lRes != ERROR_SUCCESS) 
		MessageBox(NULL, "Error open key", lpSubKey, MB_OK | MB_ICONERROR);

	lpValueName = Profile.szFriendlyName;
		
	lRes = RegDeleteValue(hkResult,  lpValueName);

	if (lRes != ERROR_SUCCESS) {
		// MessageBox(NULL, "Error Delete value 1", (LPCTSTR) lpValueName, MB_OK);
	}
		
	RegCloseKey(hkResult);
	RegCloseKey(hKey);
}



BOOL SetMenuTray(HWND hwndIcon)
{
	BOOL		bResult = FALSE;
	static int	x, y;
	
	HMENU		hMenuSysTray = CreatePopupMenu();
	Profile.hMenuSysTray = hMenuSysTray;
	bResult = SetMenu( hwndIcon, hMenuSysTray );

	x = GetSystemMetrics (SM_CXSCREEN) -50;
	y = GetSystemMetrics (SM_CYSCREEN);

	AppendMenu( hMenuSysTray, MF_STRING	  ,IDM_SETTINGS,	"&Settings...");
	AppendMenu( hMenuSysTray, MF_SEPARATOR,IDM_SEPARATOR,	NULL);
	if (Profile.bDisableWinKeys == TRUE)
		AppendMenu( hMenuSysTray, MF_STRING	  ,IDM_ENABLE,		"&Enbale Keys");
	else
		AppendMenu( hMenuSysTray, MF_STRING	  ,IDM_DISABLE,		"&Disable Keys");
	AppendMenu( hMenuSysTray, MF_SEPARATOR,IDM_SEPARATOR,  NULL);
	AppendMenu( hMenuSysTray, MF_STRING	  ,IDM_EXIT_TRAY, "&Exit");

    // This is important so that popupmenu will leave when 
    // clicked elsewhere on screen
	SetForegroundWindow( Profile.hwnd );
	TrackPopupMenuEx (hMenuSysTray, TPM_LEFTALIGN , x, y, Profile.hwnd, NULL);
	return (TRUE);
}


BOOL TrayMessage(HWND hwnd, DWORD dwMessage, UINT uID, PSTR pszTip)
{
    // Call with uID = 1 to delete icon from Win-TaskBar
	BOOL			bRes;
    HICON			hIconTray = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_APP));
	NOTIFYICONDATA	tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= hwnd;
	tnd.uID			= uID;
	tnd.uFlags		= NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnd.uCallbackMessage	= MYWM_NOTIFYICON;
	tnd.hIcon		= hIconTray;
	
	if (pszTip)
	{
		lstrcpyn(tnd.szTip, pszTip, sizeof(tnd.szTip));
	}
	else
	{
		tnd.szTip[0] = '\0';
	}

	// API - Function:
	bRes = Shell_NotifyIcon(dwMessage, &tnd);
	Profile.hwndIcon = GetWindow(hwnd, GW_HWNDNEXT);

	if (hIconTray)
	    DestroyIcon(hIconTray);

	return (bRes);
}

*/