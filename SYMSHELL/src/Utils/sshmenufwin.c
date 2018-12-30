/* IMPLEMENTATION FOR SYMSHELL MENU AND RELEATED FEATURES */
/**********************************************************/
#include "INCLUDE/platform.h"

#if defined( __WIN32__ ) 
#include <windows.h> //bo Menu itp
#include <assert.h>
#include "../../sshmenuf.h"

#ifdef __cplusplus
extern "C" 
#else
extern 
#endif
HWND	WB_Hwnd;//W symshwin.c

int ssh_set_window_name(const char* WindowName)
{
	return SetWindowText(WB_Hwnd,WindowName);//SetWindowText return TRUE on success!
}


ssh_menu_handle ssh_main_menu()//Daje uchwyt do g³ównego menu
{
return	GetMenu(WB_Hwnd);
}

ssh_menu_handle ssh_sub_menu(
					ssh_menu_handle hMenu,				
					unsigned    Position)//Daje uchwyt do podmenu wg. pozycji
{
return GetSubMenu(hMenu,Position);
}

int ssh_get_item_position(
					 ssh_menu_handle hMenu,
					 const char* ItemName) //Odnajduje pozycje itemu w jakims menu
{
	size_t len=strlen(ItemName);
	char* pom=malloc(len+1);
	int i,N=GetMenuItemCount(hMenu);
	
	for(i=0;i<N;i++)
	{
		int ret=GetMenuString(hMenu,i,pom,len+1,MF_BYPOSITION); //i ale raczej nie bêdzie ujemne	(UINT vs. size_t?)
																assert(ret!=0);
		if(strcmp(ItemName,pom)==0)
		{
			free(pom);
			return i;	
		}
	}

	free(pom);
	return -1;
}

int	ssh_menu_add_item(
					ssh_menu_handle hMenu,
					const char* ItemName,
					unsigned    Message,
					unsigned    Flags) //Dodaje item do menu
{
if(Flags==0)/* Ma byc domyslnie */
	Flags=MF_ENABLED;
//return AppendMenuW(hMenu,Flags,Message,UniItemName);
return AppendMenu(hMenu,Flags,Message,ItemName);
}

int ssh_menu_remove_item(          /* Usuwa item z menu */
					ssh_menu_handle hMenu,
					unsigned    ItemCommandOrPosition,
					unsigned    asPosition )
{
UINT Flags=0;
if(asPosition) Flags|=MF_BYPOSITION;
	else	   Flags|=MF_BYCOMMAND;
return  RemoveMenu(hMenu,ItemCommandOrPosition,Flags)!=0xffffffff;
}



int ssh_menu_mark_item(
					ssh_menu_handle hMenu,
					unsigned    Check,
					unsigned    ItemCommandOrPosition,										
					unsigned    asPosition
					   ) // Ustawia lub usuwa marker przy itemie
{
UINT Flags=0;
if(Check) Flags|=MF_CHECKED;
	else  Flags|=MF_UNCHECKED;
if(asPosition) Flags|=MF_BYPOSITION;
	else	   Flags|=MF_BYCOMMAND;
return  CheckMenuItem(hMenu,ItemCommandOrPosition,Flags)!=0xffffffff;
}

int ssh_realize_menu(ssh_menu_handle hMenu) //Zapewnia ze menu bedzie wygladac zgodnie z poprzednimi poleceniami
{
return DrawMenuBar(WB_Hwnd ); 
}

#else
#error "THIS SYMSHELL MENU IMPLEMENTATION IS NOT FOR THIS PLATFORM"
#endif

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/


