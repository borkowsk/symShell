/* KODOWANIE MENU INTEGERAMI I WSKAZNIKAMI DO LANCUCHOW */
/********************************************************/
#ifndef __SSH_MENU_DEF_H__
#define __SSH_MENU_DEF_H__

#include <stdio.h>

typedef union{unsigned u;char* p;} 	menu_optcode; /* TYP ELEMENTU MENU */
extern  menu_optcode 		  ssh_default_menu[]; /* DOMYSLNA NAZWA MENU - MUSI GDZIES BYC!*/

#define MENU		menu_optcode ssh_default_menu[]=
#define NMENU( _Name_ )	menu_optcode menu##_Name_[]=
#define MNAME( _Name_ ) menu##_Name_

/* Znak poprzedzajacy odpowiedz z serwera menu */
#define MENU_ANSWER 5 	

/* PODSTAWOWE KODY OPERACJI */
#define DISCARDABLE	 0 /*NOP=>PLACEHOLDER*/
#define NOP		 0
#define BEGIN_OPT	 1
#define BEGIN		,1
#define END_OPT		 2 
#define END		,2
#define POPUP_OPT	 3 
#define POPUP		,3,(unsigned)
#define MENUITEM_OPT	 4
#define MENUITEM	,4,(unsigned)

/* KODY SPECJALNE */
#define SELECT_OPT	5   /* WYBIERZ ODPOWIEDNI POPUP LUB ITEM*/
#define APPEND_OPT	6   /* DODAJ ITEM NA KONIEC LISTY */	
#define MODIFY_OPT	7   /* ZMIEN WLASIWOSCI ITEMU */
#define REALIZE_OPT     8   /* REALIZUJE MENU, PO MODYFIKACJI */

/* MODYFIKATORY */
#define GRAYED		10
#define CHECKED		11
#define HELP		12
#define INACTIVE	13
#define MENUBREAK	14
#define MENUBARBREAK	15

#define RESETFLAGS	19

#define NOGRAY		20
#define NOCHECK		21
#define NOHELP		22
#define ACTIVE		23
#define NOMENUBREAK	24
#define NOMENUBARBREAK	25


#define BY_MESSAGE	40   /* SZUKAJ WG KOMUNIKATU A NIE POZYCJI */
#define BY_POSITION	41  /* SZUKAJ WG POZYCJI */

#define LAST_OPTCODE	41
#define END_OF_CODES 0xffffffff

/* SPECJALNE TEKSTY */
#define SEPARATOR   "-@@-S-E-P-A-R-A-T-O-R-##-"

/* MENU INPUT/OUTPUT SUPPORT */
/*****************************/
/*   SAVING MENU TO STREAM AS TEXT                   */
void save_menu_optcodes(FILE* out,menu_optcode code[]);

/*     WRITE &  READ MENU TO/FROM STREAM AS BINARY  */
void dump_menu_optcodes(FILE* out,menu_optcode code[]);
menu_optcode* read_menu_optcodes(FILE* in);
menu_optcode* read_menu_optcodes2(int  in);

/* DEALLOCATION STRINGS IN OPTCODES AND ALL THE TABLE */
void destroy_menu_optcodes(menu_optcode* code);

#endif


