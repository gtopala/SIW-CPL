// Siw.h
#include <windows.h>
#include <Cpl.h>
#include <shlobj.h>
#include "resource.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#define NUM_APPLETS	  1	// 1 dialog box for control panel app
 
typedef struct tagApplet
{ 
    int		icon;       // icon resource identifier 
	int		namestring;	// name-string for applet
    int		descstring;	// description-string for applet
	int		dlgtempl;	// dialog box template resource identifier 
    DLGPROC dlgfn;		// dialog box procedure 
} APPLET; 


HINSTANCE	hInstance = NULL;
BROWSEINFO	bi;

__declspec(dllexport) LONG CALLBACK CPlApplet(HWND hwndCPL, UINT uMsg, LPARAM lParam1, LPARAM lParam2);
LPCTSTR GetRegKeyApp();
LPSTR BrowseFolderName (HWND hwnd);
BOOL WriteRegKeys(LPCTSTR szPath);

