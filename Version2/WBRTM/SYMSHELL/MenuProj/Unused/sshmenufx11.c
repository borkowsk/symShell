/* IMPLENETATION FOR SYMSHELL MENU AND RELEATED FEATURES FOR X11-WINDOWS */
/*****************************************************/
#include <assert.h>
#include <stdio.h>
#include <signal.h>

#include "platform.h"
#include "sshmenuf.h"
#include "sshmenud.h"

extern int ssh_menu_trace; 		/* For debuging purposes - external accesible */
extern menu_optcode ssh_default_menu[];
extern char*        ssh_menu_options;

#define DPRT {if(ssh_menu_trace) fprintf(stderr,
#define DEND   );}


FILE* MenuPipe=NULL;

int _CloseMenuPipe();

int _InitMenuPipe(unsigned long win, char* name)
{
char bufor[1024];
if(ssh_default_menu[0].u==END_OF_CODES)/* PUSTE MENU - Z BIBLIOTEKI */
    return 0;

sprintf(bufor, "sshmenux11 %lu %u -t \"%s\" %s",
			   win,ssh_menu_trace,name,ssh_menu_options);
DPRT "STARTING:%s ", bufor DEND

MenuPipe=popen(bufor, "w");
if(MenuPipe==NULL)
    {
    fprintf(stderr,"CAN`T START: %s\n", bufor);
    return 0;
    }
    
DPRT  "DONE\n" DEND

atexit(_CloseMenuPipe);
dump_menu_optcodes(MenuPipe,ssh_default_menu);
return 1;
}


menu_optcode endoftrans[]=
{
END_OF_CODES, END_OF_CODES
};

int _CloseMenuPipe()
{
if(MenuPipe)
    {
    int ret;
    signal(SIGPIPE, SIG_IGN);
    dump_menu_optcodes(MenuPipe, endoftrans );
    
    DPRT "WAIT {pipe} FOR CLOSING MENU\n" DEND
    ret=pclose(MenuPipe);/* NIESTETY TU ROBI WAIT */
    MenuPipe=NULL;

    DPRT "END OF WAITING\n" DEND
    return ret;
    }
    else  
    return  0;
}

menu_optcode get_main[]=
{NOP
BEGIN, 
SELECT_OPT,  0, 0 /* Zerowy handler i zerowa pozycja */
END    
};

ssh_menu_handle ssh_main_menu()
{
assert(sizeof(void*)<=sizeof(unsigned));
assert(sizeof(void*)<=sizeof(int));

DPRT "ssh_main_menu()\n" DEND
/* save_menu_optcodes(stderr,get_main);    DEBUG */

dump_menu_optcodes(MenuPipe, get_main);

while(get_char()!=MENU_ANSWER);		/* Czeka na sygnal */
return	(ssh_menu_handle)get_char();	/* Odsyla jako 32 bitowy "znak" */
}

menu_optcode get_sub_menu[]=
{NOP
BEGIN, 
SELECT_OPT,  -1, -1 /* Do wstawienia handler i pozycja */
END    
};

ssh_menu_handle ssh_sub_menu(
					ssh_menu_handle hMenu,				
					unsigned    Position)
{
get_sub_menu[3].u=(unsigned) hMenu;
get_sub_menu[4].u=Position;

DPRT "ssh_sub_menu(%p,%u)\n",hMenu,Position DEND

dump_menu_optcodes(MenuPipe, get_sub_menu );

while(get_char()!=MENU_ANSWER);/* Czeka na sygnal */
return	(ssh_menu_handle)get_char();/* Odsyla jako 32 bitowy "znak" */
}

menu_optcode append_menu[]=
{NOP
BEGIN, 
APPEND_OPT,  -1  	/* Do wstawienia handler */
MENUITEM    "",		/* co ITEM|POPUP  */
	    -1,		/* i jaki message do wstawienia, albo NOP lub modyfikator jesli POPUP */
NOP,NOP,NOP		/* Miejsce na trzy modyfikatory */		
END     
};

int	ssh_menu_add_item(
					ssh_menu_handle hMenu,
					const char* ItemName,
					unsigned    Message,
					unsigned    Flags)
{
append_menu[3].u=(unsigned) hMenu;
append_menu[5].p=(char*)ItemName;
append_menu[6].u=Message;

if(Flags!=0)/* Nie ma byc domyslnie */
	{
	/* ... */
	}
	
DPRT "ssh_menu_add_item(%p,%s,%u,%u)\n",hMenu,ItemName,Message,Flags DEND

dump_menu_optcodes(MenuPipe, append_menu );

while(get_char()!=MENU_ANSWER);/* Czeka na sygnal */
return	get_char()!=0;/* Odsyla status */
}


menu_optcode modify_menu[]=
{NOP
BEGIN, 
MODIFY_OPT,  -1,  -1 ,  /* Do wstawienia handler i message/pozycja */
BY_MESSAGE , 
-1 			/* Modyfikator do wstawienia */
END     
};

int ssh_menu_mark_item(
					ssh_menu_handle hMenu,
					unsigned    Check,
					unsigned    ItemCommandOrPosition,										
					unsigned    asPosition
					   )
{
unsigned Flags=0;
modify_menu[3].u=(unsigned) hMenu;
modify_menu[4].u=ItemCommandOrPosition;

DPRT "ssh_menu_mark_item(%p,%u,%u,%u)\n",
	hMenu,Check,ItemCommandOrPosition,asPosition
DEND

if(asPosition) modify_menu[5].u=BY_POSITION;
	else   modify_menu[5].u=BY_MESSAGE;
	
if(Check) modify_menu[6].u=CHECKED;
	else modify_menu[6].u=RESETFLAGS;

dump_menu_optcodes(MenuPipe, modify_menu);

while(get_char()!=MENU_ANSWER);/* Czeka na sygnal */
return	get_char()!=0;/* Odsyla status */
}


menu_optcode realize_menu[]=
{NOP
BEGIN, 
REALIZE_OPT,  -1  /* Do wstawienia handler */
END     
};

int ssh_realize_menu(ssh_menu_handle hMenu)
{
DPRT "ssh_realize_menu(%p)\n",hMenu DEND

realize_menu[3].u=(unsigned)hMenu;
dump_menu_optcodes(MenuPipe,realize_menu );

while(get_char()!=MENU_ANSWER);/* Czeka na sygnal */
return	get_char()!=0;/* Odsyla status */
}


/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/


