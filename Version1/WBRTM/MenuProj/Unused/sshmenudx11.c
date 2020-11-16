/*******************************************************************/
/*  SYMSHELL MENU SUPPORT - IMPLEMENTATION OF OPTCODES INTERFACE   */
/*******************************************************************/
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#include "sshmenud.h"/* SYMSHELL MENU HEADER */

/*   LOCAL ERROR HANDLER    */
static void fatal(char* text)
/****************************/
{
fprintf(stderr, "SSHMENU INTERFACE ERROR: %s %s\n", text,strerror(errno));
#ifdef NDEBUG
exit(1);   
#else
abort();
#endif
}


/*          SAVING MENU TO STREAM                     */
void save_menu_optcodes(FILE* out,menu_optcode code[])
/******************************************************/
{
int  nawiasy=-1,licznik=0;
int before=0;
while(1)
	{
	switch(code[licznik].u)
		{
	case END_OF_CODES:
		putc('\004', out);/* Ctrl D */
		goto Koniec;
	case POPUP_OPT:
		fprintf(out,"%u ",code[licznik++].u);
		fprintf(out,"\"%s\" ",code[licznik].p);/*Nazwa*/
		break;
	case MENUITEM_OPT:
		fprintf(out,"%u ",code[licznik++].u);
		fprintf(out,"\"%s\" ",code[licznik].p);/*Nazwa*/
		if(strcmp(code[licznik].p, SEPARATOR)!=0)
		    fprintf(out,"%u ",code[++licznik].u);/* KOD */
		break;
	case END_OPT:
		fprintf(out,"%u ",code[licznik].u);
		if(--nawiasy<0) goto Koniec;
			else break;
	case SELECT_OPT:   /* WYBIERZ ODPOWIEDNI POPUP LUB ITEM*/
	case MODIFY_OPT:   /* ZMIEN WLASIWOSCI ITEMU */
	    /*SA DWUPARAMETROWE - HANDLER I POZYCJA */
		fprintf(out,"%u ",code[licznik++].u);
		fprintf(out,"%u ",code[licznik++].u);
		fprintf(out,"%u ",code[licznik].u);
		break;
	case APPEND_OPT:   /* DODAJ ITEM NA KONIEC LISTY */	 
	case REALIZE_OPT:  /* REALIZUJE MENU, PO MODYFIKACJI */
		/*SA JEDNOPARAMETROWE - TYLKO HANDLER */
		fprintf(out,"%u ",code[licznik++].u);
		fprintf(out,"%u ",code[licznik].u);
		break;	
	case BEGIN_OPT:
		nawiasy++;
	case  NOP:
	case  GRAYED:
	case  CHECKED:
	case  HELP:
	case  INACTIVE:
	case  MENUBREAK:
	case  MENUBARBREAK:
	case  RESETFLAGS:	
	case  NOGRAY:		
	case  NOCHECK:	
	case  NOHELP:		
	case  ACTIVE:		
	case  NOMENUBREAK:	
	case  NOMENUBARBREAK:
	case  BY_MESSAGE:
	case  BY_POSITION:
		 fprintf(out,"%u ",code[licznik].u);
		 break;
	default:
		if(code[licznik].u<LAST_OPTCODE) 
			fprintf(stderr,"Invalid menu optcode %u - skipped\n",code[licznik].u);
			else
			fprintf(out,"%u ",code[licznik].u);
		}
	licznik++;
	}
Koniec: 
	 fprintf(out,"\n");
	 fflush(out);
}

#define EECHO(_P_) fprintf(stderr, _P_ )
/*         DUMPING MENU TO STREAM                     */
/******************************************************/
void writeu(FILE* out,unsigned pom)
{
size_t ret=0;
/* fprintf(stderr,"WR: %u\n", pom);  DEBUG */
if((ret=fwrite(&pom,sizeof(unsigned),1,out))!=1)
	fatal("Write unsigned foult\n");
}
void writes(FILE* out,char* pom)
{
size_t len=strlen(pom);
writeu(out,len+1);
/* fprintf(stderr,"WR: %s\n", pom); DEBUG */
if(fwrite(pom,sizeof(char),len+1,out)!=sizeof(char)*(len+1))
		fatal("Write string foult\n");
}
void dump_menu_optcodes(FILE* out,menu_optcode code[])
/******************************************************/
{
int  nawiasy=-1,licznik=0;
int before=0;
while(1)
	{
	switch(code[licznik].u)
		{
	case END_OF_CODES:
		writeu(out,code[licznik].u);
		goto Koniec;
	case POPUP_OPT:
	        writeu(out,code[licznik++].u);
		writes(out,code[licznik].p);
		break;
	case MENUITEM_OPT:
		writeu(out,code[licznik++].u);
		writes(out,code[licznik].p);
		if(strcmp(code[licznik].p, SEPARATOR)!=0)
		    writeu(out,code[++licznik].u);/* KOD */
		break;
	case END_OPT:
		writeu(out,code[licznik].u);
		if(--nawiasy<0) goto Koniec;
			else break;	
	case SELECT_OPT:   /* WYBIERZ ODPOWIEDNI POPUP LUB ITEM*/
	case MODIFY_OPT:   /* ZMIEN WLASIWOSCI ITEMU */
	    /*SA DWUPARAMETROWE - HANDLER I POZYCJA */
		writeu(out,code[licznik++].u);
		writeu(out,code[licznik++].u);
		writeu(out,code[licznik].u);
		break;
	case APPEND_OPT:   /* DODAJ ITEM NA KONIEC LISTY */	 
	case REALIZE_OPT:  /* REALIZUJE MENU, PO MODYFIKACJI */
		/*SA JEDNOPARAMETROWE - TYLKO HANDLER */
		writeu(out,code[licznik++].u);
		writeu(out,code[licznik].u);
		break;
	case BEGIN_OPT:
		nawiasy++;	
	case  NOP:
	case  GRAYED:
	case  CHECKED:
	case  HELP:
	case  INACTIVE:
	case  MENUBREAK:
	case  MENUBARBREAK:	
	case  RESETFLAGS:	
	case  NOGRAY:		
	case  NOCHECK:	
	case  NOHELP:		
	case  ACTIVE:		
	case  NOMENUBREAK:	
	case  NOMENUBARBREAK:
	case  BY_MESSAGE:
	case  BY_POSITION:
		 writeu(out,code[licznik].u);
		 break;
	default:
		if(code[licznik].u<LAST_OPTCODE) 
			fprintf(stderr,"Invalid menu optcode %u - skipped\n",code[licznik].u);
			else
			writeu(out,code[licznik].u);
		}
	licznik++;
	}
Koniec: 
	 fflush(out);
}

/*       READ MENU FROM DUMPED STREAM    */
/******************************************************/
unsigned readu(FILE* in)
{
unsigned pom=0;
if(fread(&pom,sizeof(unsigned),1,in)!=1)
	fatal("Read unsigned foult\n");
/* fprintf(stderr,"RD: %u\n", pom); DEBUG */
return pom;
}
char* reads(FILE* in)
{
size_t len=readu(in);
char* pom=malloc(len);
if(len==0)
	fatal("String must be 1 char or longer!");
if(pom==NULL)
	fatal("Alloc string foult");
if(fread(pom,sizeof(char),len,in)!=sizeof(char)*(len))
	fatal("Read string foult\n");
/* fprintf(stderr,"RD: %s\n", pom); DEBUG */
return pom;
}
#define REALLOCATION  /*ALOKACJA BUFORA*/{ \
	menu_optcode* pom=NULL;			\
	size_t oldsize=size;			\
	if(size==0) size=4;				\
		else   size*=2;				\
	pom=calloc(size,sizeof(menu_optcode));\
	if(pom==NULL)										\
		fatal("Cant allocate bufor for menu optcodes");	\
	if(oldsize>0)/* PRZEPISANIE ZAWARTOSCI */			\
		memcpy(pom,code,oldsize*sizeof(menu_optcode));	\
	code=pom;/* ...I WSKAZNIKA */						\
	}													
menu_optcode* read_menu_optcodes(FILE* in)
/******************************************************/
{
menu_optcode* code=NULL;
size_t		  size=0;	
size_t		  licz=0;
int			nawiasy=-1;

	do{
	unsigned opt=readu(in);	
	if(licz==size)
		REALLOCATION;
	code[licz].u=opt;	
	licz++;

	switch(opt){
	case END_OPT:
		if(--nawiasy<0) goto Koniec;
			else break;
	case BEGIN_OPT:
		nawiasy++;
		break;
	case SELECT_OPT:  
	case MODIFY_OPT:  
		{
	    /*SA DWUPARAMETROWE - HANDLER I POZYCJA */
	    /*Parametry moga pokrywac sie z optcodami! */
	        if(licz==size)
			REALLOCATION;
		code[licz].u=readu(in);
		licz++; 
		if(licz==size)
			REALLOCATION;
		code[licz].u=readu(in);
		licz++;
		}
		break;
	case APPEND_OPT:   	 
	case REALIZE_OPT: 
		{
		/*SA JEDNOPARAMETROWE - TYLKO HANDLER */ 
		/*Parametr moze pokrywac sie z optcodami! */
		if(licz==size)
			REALLOCATION;
		code[licz].u=readu(in);
		licz++;
		}
		break;	
	case POPUP_OPT: /* Jednoparametrowy - nazwa */
		{
		char* pom=reads(in);
		if(licz==size)
			REALLOCATION;
		code[licz].p=pom;	
		licz++;
		}
		break;
	case MENUITEM_OPT: /* Dwuparametrowy - nazwa + kod */
		{	   /* chyba ze jest MENUITEM SEPARATOR */
		char* pom=reads(in);
		if(licz==size)
			REALLOCATION;
		code[licz].p=pom;
		if(strcmp(code[licz].p,SEPARATOR)!=0)
		    {	
		    licz++;
		    if(licz==size)
			REALLOCATION;
		    code[licz].u=readu(in);/* KOD DO ZWRACANIA */
		    }
		licz++;
		}
		break;	
	case END_OF_CODES:
		/* JEST JUZ WPISANE */
		goto Koniec;
	}/*switch*/

	}while(1);
Koniec:
return code;
}

#undef REALLOCATION

/* DEALLOCATION STRINGS IN OPTCODES AND ALL THE TABLE */
void destroy_menu_optcodes(menu_optcode* code)
/******************************************************/
{
size_t		licz=0;
int		nawiasy=-1;

	do{
	unsigned opt=code[licz].u;	
	licz++;
	switch(opt){
	case END_OPT:
		if(--nawiasy<0) goto Koniec;
			else break;
	case BEGIN_OPT:
		nawiasy++;
		break;
	case SELECT_OPT:  
	case MODIFY_OPT:  
	    /*SA DWUPARAMETROWE - HANDLER I POZYCJA */
	    /*Parametry moga pokrywac sie z optcodami! */	   
		licz+=2;/* Przeskok! */ 
		break;
	case APPEND_OPT:   	 
	case REALIZE_OPT: 
		/*SA JEDNOPARAMETROWE - TYLKO HANDLER */ 
		/*Parametr moze pokrywac sie z optcodami! */
		licz++;
		break;	
	case POPUP_OPT:
		{
		char* pom=code[licz].p;	
		/* fprintf(stderr, "Dealocation of popup: %s\n", pom); DEBUG */
		code[licz].p=NULL;
		free(pom);/* DELETE MENU STRING */
		licz++;
		}
		break;
	case MENUITEM_OPT:
		{
		char* pom=code[licz].p;	
		/* fprintf(stderr, "Dealocation of menu item: %s\n", pom); DEBUG */
		code[licz].p=NULL;
		
		if(strcmp(pom, SEPARATOR)!=0)	
		    licz+=2;		   
		    else
		    licz++;
		   
		free(pom);/* DELETE MENU STRING */   
		}
		break;	
	case END_OF_CODES:
		goto Koniec;
	}/*switch*/

	}while(1);
Koniec:
free(code);/* DELETE OPTCODES TABLE */
}
