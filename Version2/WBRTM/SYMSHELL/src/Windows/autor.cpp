#include <windows.h>
#include "SYMSHELL/src/Windows/symshwin.h"            // prototypes specific to this application

#define X( _p_ )     ((_p_)^'%')
extern char SymShellProd[];
//"Windows Symshell . Compiled "__DATE__ __TIME__;
static char WB_Copyright[]=
//!W!o!j!c!i!e!c!h! !T!.! !B!o!r!k!o!w!s!k!i! !f!r!o!m! !W!a!r!s!a!w! !U!n!i!v!e!r!s!i!t!y!
{
X(' '),
X('W'), X('o'), X('j'), X('c'), X('i'), X('e'), X('c'), X('h'), 
X(' '), X('T'), X('.'), X(' '), 
X('B'), X('o'), X('r'), X('k'), X('o'), X('w'), X('s'), X('k'), X('i'), 
X(' '), X('f'), X('r'), X('o'), X('m'), X(' '), 
X('W'), X('a'), X('r'), X('s'), X('a'), X('w'), X(' '), 
X('U'), X('n'), X('i'), X('v'), X('e'), X('r'), X('s'), X('i'), X('t'), X('y'),
0
};

extern "C"
int wb_about(char* window_name)
{
static char WB_date[]="Copyright © "__DATE__;
char bufor[256];
extern HINSTANCE WB_Instance;
extern HWND		 WB_Hwnd;
HICON hIcon;
strcpy(bufor,WB_date);

{//Append coded Copyright
char* pom1=bufor+strlen(WB_date);
const char* pom2=WB_Copyright;
while(*pom2!='\0')
		*pom1++=X(*pom2++);
*pom1='\0';
}

hIcon=LoadIcon(WB_Instance, MAKEINTRESOURCE(IDI_APPICON));
ShellAbout(WB_Hwnd,window_name,bufor,hIcon);

DeleteObject(hIcon);
return 0;
}

#undef X
