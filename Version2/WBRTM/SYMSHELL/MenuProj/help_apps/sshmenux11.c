/*******************************************************************/
/*  SYMSHELL MENU SUPPORT FOR X11/MOTIF/...                        */
/*******************************************************************/
/* Standard C code!!! */

int ssh_menu_trace=0; /* For debuging purposes - external accesible */

/* Standard C headers */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

/* Standard Toolkit files */
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/bitmaps/xlogo16>

#ifdef MOTIF

#include <Xm/Xm.h>
#include <Xm/RepType.h>
/* Public include files for widgets used in this file. */
#include <Xm/PushB.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/MessageB.h>
#include <Xm/CascadeB.h>
#include <Xm/Frame.h>

#elif defined ATHENA
/* ATENA 3D pomiewaz zwykla ma BLAD */
#include <X11/Xaw3d/Box.h>
#include <X11/Xaw3d/MenuButton.h>
#include <X11/Xaw3d/SimpleMenu.h>
#include <X11/Xaw3d/SmeBSB.h>
#include <X11/Xaw3d/SmeLine.h>
static Pixmap mark;

#else
#error SPECIFY WIDGET SET 'MOTIF' or 'ATHENA'
#endif


/* SYMSHELL MENU OPTCODES & COMMUNICATION HEADER */
#include "sshmenud.h"

unsigned long Target=0;	/* ID OF TARGET WINDOW */
Display* MyDisplay=0;   /*=XtDisplay(w); MUSI BYC NA TYM SAMYM DISPLAYU CO KLIENT */

#define DPRT {if(ssh_menu_trace){/*fprintf(stderr,"\nsshmenux11: "); */fprintf(stderr,
#define DEND   );}}

/*   LOCAL ERROR HANDLER    */
static void fatal(char* text)
/****************************/
{
fprintf(stderr, "SSHMENUX11 FATAL ERROR: %s %s\n", text,strerror(errno));
exit(1);   
}

/* HELPING FUNCTION FOR STRING CLONING */
static char* clone_str(const char* what)
/***************************************/
{
size_t len=strlen(what);
char*  pom=malloc(len+1);
if(pom==NULL) fatal("Text allocation failed");   
strcpy(pom, what);
return pom;
}

/* HELPING FUNCTION FOR TABULATING OUTPUT */
static void tabul(FILE* out,int level)
/******************************************/
{
int i;
for(i=level;i>0;i--)
    putc('\t',out);    
}

/* HELPING FUNCTION FOR SENDING UINT32 TO CLIENT PROCESS */
void Send32(unsigned int code32)
/*********************************************************/
{
XClientMessageEvent MyEvent;
MyEvent.type=ClientMessage;
MyEvent.format=32;
/*strcpy(MyEvent.data.b, "/text/testowy456789"); * DO 20 znakow!!!! */
MyEvent.data.l[0]=code32;

DPRT "Send32 0x%x==%u to window 0x%lx; ",code32,code32, Target DEND

if(XSendEvent(MyDisplay,Target,FALSE,  
	   ExposureMask | KeyPressMask | 
	   ButtonPressMask | StructureNotifyMask,
	   (XEvent*)&MyEvent)==0)
		fprintf(stderr," SEND EVENT 32 FAILED!\n");   
}

void Send16(unsigned short int code16)
/*********************************************************/
{
XClientMessageEvent MyEvent;
MyEvent.type=ClientMessage;
MyEvent.format=16;
MyEvent.data.l[0]=code16;

DPRT "Send16 0x%x==%u to window 0x%lx; ",code16,code16, Target DEND

if(XSendEvent(MyDisplay,Target,FALSE,  
	   ExposureMask | KeyPressMask | 
	   ButtonPressMask | StructureNotifyMask,
	   (XEvent*)&MyEvent)==0)
		fprintf(stderr," SEND EVENT 16 FAILED!\n");   
}

/* HANDLER FOR CLOSING MENU */
void ForExitTopLevel(Widget w,XtPointer client_data,XtPointer call_data)
/***********************************************************************/
{
Send32(3);/* ctrl C */
Send32(3);/* ctrl C */
fprintf(stderr,"\n ssshmenux11: ForExitTopLevel called.\n"); /* DEBUG */		        
}


/* STRUCT FOR WORKABLE DATA OF MENU ITEM */
/*****************************************/

#define MARKER 0xAbcdef01
typedef struct menu_item
/*****************************************/
{
unsigned 	Marker;/* == 0xAbcdef01 */
int		Modified:1;
int 		checked:1;
int 		grayed:1;
int 		inactive:1;
struct menu_item* next;
struct menu_item* down;
unsigned 	code;
char*    	text;
Widget   	wid;
Widget   	parent;
Widget		sub;
} menu_item;


/* CALLBACK FUNCTION FOR MENU ITEM WIDGET                           */
void MenuCallback(Widget w,XtPointer client_data,XtPointer call_data)
/********************************************************************/
{
   menu_item* pom = (menu_item*) client_data;
   
   /* Kiedy sie rezygnuje z akcji */
   if(pom==NULL) return;
   assert(pom->Marker==MARKER);
   
   if(pom->code==0) return;
   if(pom->inactive || pom->grayed) return;
   
   DPRT "Selected %s. ", pom->text DEND
   if(!pom->inactive)
		{
		Send32(pom->code); 
		}
		
   DPRT "\n" DEND
}

/* RESOURCES SETTINGS - UPDATE */
void update_menu_widget(menu_item* item)
/*******************************************/
{
      Pixel    White;
      Pixel    Black;
      
      DPRT "Update menu item " DEND 
     
      if(item==NULL) return;
      assert(item->Marker==MARKER);
      
      DPRT "\"%s\" ",item->text DEND
      
      if(item->wid==NULL) return; /* nie tym razem */
      
      /* Mozliwe wartosci */
      White=WhitePixelOfScreen(XtScreen(item->wid));
      Black=BlackPixelOfScreen(XtScreen(item->wid));
      
      /* USTAWIANIE */
#ifdef MOTIF
      {
      char bufor[1024];
      
      if(item->checked)
      	sprintf(bufor,"*%s",item->text);
      	else
      	sprintf(bufor," %s",item->text);
      	
      Label=XmStringCreateSimple(bufor);
      XtVaSetValues(item->wid,
		  	XmNlabelString,Label, 
		  	XmNforeground,item->grayed?White:Black,NULL);
      if(item->inactive)
          XtVaSetValues(item->wid,	  	
		  	XmNbackground,White,
		  	NULL);
      }
#else /*ATHENA*/
      {
      if (item->checked) 
        XtVaSetValues(item->wid, 
                XtNleftBitmap, mark,
                NULL);
	    else 
        XtVaSetValues(item->wid, 
                XtNleftBitmap, None,
                NULL);
   
      XtVaSetValues(item->wid,
		  	XtNforeground,
			item->grayed?White:Black,
			NULL);
			
      if(item->inactive)
          XtVaSetValues(item->wid,	  	
		  	XtNbackground,
			White,
		  	NULL);
			
      XtVaSetValues(item->wid,	  	
		  	XtNsensitive,
			!(item->inactive || item->grayed),
		  	NULL);
		
      }
#endif			
      item->Modified=0;/* Updated */
      
      DPRT "DONE\n" DEND
}

/* MAKE WIDGET FOR MENU ITEM */
void make_widget(menu_item* item, Widget parent)
/*************************************************/
{
   char bufor[1024];
   DPRT "Make widget " DEND
   
   if(parent==NULL) return; /* Nie mozna nic zrobic! */
      
   if(item->text!=NULL && item->wid==NULL) /* Czy jest tekst, a nie ma Widgetu */
      {
      DPRT "for \"%s\" ",item->text DEND
      
      sprintf(bufor, " %s",item->text);
       
      item->parent=parent;		/* Zapamientanie "ojca" */
      
      item->wid=XtCreateManagedWidget(
	    bufor,			/* arbitrary widget name */
#ifdef MOTIF
	    xmCascadeButtonWidgetClass, /* niewypukly przycisk - jako element menu */
#else
	    item->down!=NULL?
		menuButtonWidgetClass:	/* ATHENA menu-trigger button */
		XtIsSubclass(parent,simpleMenuWidgetClass)?
		    smeBSBObjectClass:	/* Buttun for menu */
		    commandWidgetClass,	/* simple ATHENA Button */
#endif
	    parent,			/* parent widget*/
	    NULL,			/* argument list */
	    0);				/* arg list size */
      
      if(item->wid==NULL) 
	{
	fatal("Widget construction failed.");
	return; /* ERROR */	
	}
	
      /* arrange for button to call function */
      XtAddCallback(item->wid, 
#ifdef MOTIF      
		    XmNactivateCallback, 
#else
		    XtNcallback, 
#endif
		    MenuCallback, 
		    item);
      
      item->Modified=1;
      
      DPRT "DONE\n" DEND      
      }
}

/* MAKE WIDGET FOR MENU ITEM */
Widget make_popup(menu_item* item, Widget parent)
/*************************************************/
{
    DPRT "Make popup " DEND

    if(parent==NULL) return NULL; /* Nie mozna nic zrobic! */

    if(item->text!=NULL && item->sub==NULL)  /* Czy jest tekst, a nie ma SubMenu */  
	{
	Widget SubMenu=NULL;
	DPRT "for \"%s\" ",item->text DEND
	
#ifdef MOTIF
	/* CREATE MENU (REALLY A SHELL WIDGET AND ROWCOLUMN WIDGET COMBO) */
	SubMenu = XmCreatePulldownMenu(
            		parent,     /* parent widget	*/
            		item->text, /* widget name 	*/
            		NULL,       /* no argument list needed */
            		0);         /* no argument list needed */
	
	if(SubMenu==NULL) return NULL;
	
	/* SPECIFY WHICH MENU BUTTON WILL POP UP.*/
	XtVaSetValues(item->wid,XmNsubMenuId, SubMenu,NULL);
#else
	 {
	 char menu_name[1024]="menu";
	 /*sprintf(menu_name, "menu-%s",item->text); */
	 DPRT "\"%s\"",menu_name  DEND
	 SubMenu = XtCreatePopupShell(
			    menu_name, 
			    simpleMenuWidgetClass, 
			    item->wid,  /*button*/ 
			    NULL,
			    0);
			    
	if(SubMenu==NULL) return NULL;	
	/*XtVaSetValues(SubMenu,XtNlabelClass,commandWidgetClass, NULL);*/ 	    
	/*XtVaSetValues(item->wid,XtNmenuName,menu_name,NULL);*/		    
	}
#endif		  	
	item->sub=SubMenu;
	
	DPRT "DONE\n" DEND 	  
	return SubMenu;
	}

    return NULL;			  				
}

/* RECURENT FUNCTION FOR REALIZE item-CODED-MENU */
void update_menu_tree(menu_item* tree, Widget parent)
/*************************************************/
{
    while(tree!=NULL)
	{
	if(tree->wid==NULL)
		make_widget(tree, parent);
	
	if(tree->Modified)
		update_menu_widget(tree);
    	
	if(tree->down!=NULL && tree->text!=NULL )	/* TO JEST POPUP! */
		{
		if(tree->sub==NULL)
			make_popup(tree,parent);

		update_menu_tree(tree->down,tree->sub);	 	    
		}	    

	/*XtManageChild(tree->wid);*/
	
	/***GO TO NEXT ITEM***/
	NEXT:
	tree=tree->next;
	}
}

/* MENU ITEM ALLOCATION */
menu_item* alloc_item()
/************************/
{
menu_item*  item=malloc(sizeof(menu_item));
if(item==NULL) fatal("MenuItem allocation failed");
item->Marker=MARKER;
item->Modified=1;
item->checked=0;
item->grayed=0;
item->inactive=0;
item->next=NULL;
item->down=NULL;
item->code=0;
item->text=NULL; 
item->wid=NULL; 
item->sub=NULL;
item->parent=NULL;
return item;
}

/* ...AND DEALOCATION   */
void free_item(menu_item* item)
/*****************************/
{
if(item==NULL) return;
assert(item->Marker==MARKER);
DPRT "Dealocate \"%s\"\n",item->text DEND

free_item(item->down);

free_item(item->next); 

if(item->text!=NULL) free(item->text);
free(item);  
}

/* SEARCHING ITEM BY INDEX */
menu_item* search_position(menu_item* ptr/* Pierwszy item */,unsigned position)
/*****************************************************************************/
{
/* Szukanie odpowiedniego itemu w menu wskazanym przez ptr*/
while(position>0 && ptr!=NULL) 
	{
	ptr=ptr->next;
	position--;
	}
return ptr;
}

/* SEARCHING LAST ITEM */
menu_item* search_last(menu_item* ptr)
/*****************************************************************************/
{
/* Szukanie ostatniego poprawnego itemu */
while(ptr->next!=NULL) 
	{
	ptr=ptr->next;
	}
return ptr;
}	

/* SEARCHING ITEM BY MESSAGE */
menu_item* search_message(menu_item* ptr,unsigned messg)
/*****************************************************************************/
{
while(ptr!=NULL) 
	{
	menu_item* pom=NULL;
	if(ptr->code==messg) return ptr;
	if((pom=search_message(ptr->down,messg))!=NULL) return pom;
	ptr=ptr->next;
	}
return NULL;
}

/* HELPER DEBUGING PRINT MENU ITEM   */
void print_item(FILE* out,menu_item* item, int level)
/*****************************************/
{
tabul(out,level);

if(item==NULL) 
    {fprintf(out,"{NULL}\n");return;}
assert(item->Marker==MARKER);

fprintf(out,"{ \"%s\" \t %d ", item->text?item->text:"NULL", item->code);
if(item->checked!=0) fprintf(out, " CHECKED ");
if(item->grayed!=0) fprintf(out, " GRAYED");
if(item->inactive!=0) fprintf(out, " INACTIVE");

fprintf(out, "\n");
if(item->down!=NULL)
    print_item(out, item->down, level+1);

tabul(out,level);
fprintf(out, "}\n");
if(item->next!=NULL)
    print_item(out, item->next, level);
}

/*       RECURENT CONSTRUCT MENU FROM OPTCODES              */
menu_item* compose_menu(menu_optcode menu[], size_t* licznik)
/************************************************************/
{
menu_item pom={0, 0, 0, 0, 0, 0, 0};
menu_item* last=NULL;

/* BUDOWANIE MENU  Z OPTKODOW */
size_t i=(licznik!=NULL?*licznik:0);
while(menu[i].u!=BEGIN_OPT)/* SEARCH BEGIN */
	i++;

/* CURRENT IS ROOT */	
last=&pom;
i++;

/* PARSE ITEMS */	
do
{
switch(menu[i].u){
case END_OPT:/* END OF DEFINITION */
    if(licznik!=NULL)
	    *licznik=i;
    return pom.next;

case MENUITEM_OPT:
    last->next=alloc_item();
    last=last->next;
    i++;  
    if(strcmp(menu[i].p,SEPARATOR)!=0)
	{
	last->text=clone_str(menu[i].p);
	i++;
	last->code=menu[i].u;	
	}
	else
	last->text=clone_str("");
    break;

case POPUP_OPT:
    last->next=alloc_item();
    last=last->next;
    i++;
    last->text=clone_str(menu[i].p);
    i++;
    last->down=compose_menu(menu, &i);
    break;

case GRAYED:
    last->grayed=1;
    break;	

case CHECKED:
    last->checked=1;
    break;		

case HELP:
    break;		

case INACTIVE:
    last->inactive=1;
    break;	

case MENUBREAK:
    break;	

case MENUBARBREAK:
    break;	  

default:
    fprintf(stderr, "Ignore %x\n", menu[i].u);	/* DEBUG? */
    };
i++;
}while(1);

return NULL;
}


/*      MODIFICATION OF ALREADY CONSTRUCTED MENU TREE     */
menu_item* modify_menu(menu_item* root,menu_optcode menu[])
/**********************************************************/
{
size_t licznik=0;
menu_item* ret_ptr=NULL;

while(menu[licznik].u!=BEGIN_OPT)/* SEARCH BEGIN */
	{
	DPRT "%s %u\n","SKIP",menu[licznik].u DEND
	licznik++;
	}
	
DPRT "%s\n","BEGIN" DEND
licznik++;/*SKIP BEGIN*/

do
{
switch(menu[licznik++].u){

case SELECT_OPT:
    {
    unsigned position=0;
    
    ret_ptr =menu[licznik++].u; /* Odczyt polecenia */
    position=menu[licznik++].u;
    
    DPRT "%s %p %u\n","SELECT_OPT",ret_ptr,position DEND
    
    /* Interpretacja zlecenia */
    if(ret_ptr==NULL && position==0)
	{
	ret_ptr=root;
	DPRT "%s\n","{Main menu}" DEND
	goto Koniec;
	}
    
    if(ret_ptr==NULL) 
    		goto Koniec;/* Bledne uzycie */

    if(ret_ptr->Marker!=MARKER) 
    		{
    		ret_ptr=NULL;
    		goto Koniec;/* Bledne dane */ 
    		}
    
    /* Szukanie odpowiedniego itemu w menu wskazanym przez ret_ptr*/
    ret_ptr=search_position(ret_ptr,position);
    if(ret_ptr!=NULL && ret_ptr->down!=NULL) /* Jest w ogole i jest popupem */
    	{
    	ret_ptr=ret_ptr->down;
	if(ssh_menu_trace)
    	    print_item(stderr, ret_ptr,0);  	/* DEBUG */
    	}
    	else
    	{
    	ret_ptr=NULL;
    	}
    goto Koniec; 		/* Znaleziony albo NULL */	
    }
    break;
   
case APPEND_OPT:
    { 
    menu_item* save_ptr=NULL;   
    ret_ptr=menu[licznik++].u; 	/* Odczyt wskaznika do menu*/
    DPRT "%s 0x%p\n","APPEND_OPT",ret_ptr DEND
    
    if(ret_ptr==NULL) 
    		goto Koniec;	/* Bledne uzycie */

    if(ret_ptr->Marker!=MARKER) 
    		{
    		ret_ptr=NULL;
    		goto Koniec;	/* Bledne dane */ 
    		}
    		
    save_ptr=ret_ptr;		
    if(ret_ptr->text!=NULL)	/* Jesli niepusty to trzeba alokowac nastepny */
    	{
    	ret_ptr=search_last(ret_ptr);    	
    	ret_ptr->next=alloc_item();
    	ret_ptr=ret_ptr->next;
    	}
	
    if(ssh_menu_trace)
	print_item(stderr, save_ptr,0);  	/* DEBUG */
    }
    break;
    
case MENUITEM_OPT:
    { /* Zapisywanie odbywa sie na ret_ptr */
    char* tresc=NULL;
    unsigned messg=-1;
    tresc=menu[licznik++].p;		/* Tekst itemu */
    messg=menu[licznik++].u;		/* Skojarzona wartosc komunikatu */
    DPRT "%s \"%s\"->%u\n","MENUITEM_OPT",tresc,messg DEND
    
    if(strcmp(tresc,SEPARATOR)!=0)
	{
	ret_ptr->text=clone_str(tresc);
	ret_ptr->code=messg;	
	}
	else
	ret_ptr->text=clone_str("");
    
    }
    break;
    
case POPUP_OPT:
    {
    char* tresc=menu[licznik++].p;		/* Nazwa sub-menu */
    DPRT "%s \"%s\"\n","POPUP",tresc DEND	
    ret_ptr->text=clone_str(tresc);
    ret_ptr->down=alloc_item();
    }
    break;
    
case MODIFY_OPT:
    {
    unsigned inter, position;
    ret_ptr=menu[licznik++].u; 		/* Odczyt wskaznika do menu*/
    position=menu[licznik++].u;		/* Pozycja w menu */
    inter=menu[licznik++].u;		/* Jak interpretowac pozycje */
    DPRT "%s 0x%p[%u %s]\n","MODIFY_OPT",ret_ptr,position,(inter==BY_POSITION?"BY_POSITION":"BY_MESSAGE") DEND
    
    if(ret_ptr==NULL) 
    		goto Koniec;/* Bledne uzycie */

    if(ret_ptr->Marker!=MARKER) 
    		{
    		ret_ptr=NULL;
    		goto Koniec;/* Bledne dane */ 
    		}
    		
    if(inter==BY_POSITION)
    	ret_ptr=search_position(ret_ptr,position);
    	else    		
    	ret_ptr=search_message(ret_ptr,position);
    	
    if(ssh_menu_trace)
	print_item(stderr, ret_ptr,0);  	/* DEBUG */
    }
    break;
    
case RESETFLAGS:
    DPRT "%s ","RESETFLAGS" DEND
    if(ret_ptr)
    	{
    	ret_ptr->grayed=0;
    	ret_ptr->checked=0;
    	ret_ptr->inactive=0;
    	ret_ptr->Modified=1;
    	}
    break;
    
case GRAYED:
    DPRT "%s ","GRAYED" DEND
    if(ret_ptr)
    	{
    	ret_ptr->grayed=1; 
    	ret_ptr->Modified=1;
    	}
    break;
    
case CHECKED:
    DPRT "%s ","CHECKED" DEND
    if(ret_ptr)
    	{
    	ret_ptr->checked=1;
    	ret_ptr->Modified=1;
    	}
    break;	
    	
case INACTIVE:
    DPRT "%s ","INACTIVE" DEND
    if(ret_ptr)
    	{
    	ret_ptr->inactive=1;
    	ret_ptr->Modified=1;
    	}
    break;	
    
case HELP:
    DPRT "%s ","HELP" DEND
    break;
    
case MENUBREAK:
    DPRT "%s ","MENUBREAK" DEND
    break;	
    
case MENUBARBREAK:
    DPRT "%s ","MENUBARBREAK" DEND
    break;	
        
case REALIZE_OPT: 
    ret_ptr=menu[licznik++].u; 		/* Odczyt wskaznika do menu*/
    DPRT "%s 0x%p\n","REALIZE_OPT",ret_ptr DEND
    /* ... */
    break;

case NOP:
    DPRT "%s ","NOP" DEND
    break;

case END_OPT:	/* END OF DEFINITION */
    DPRT "\n%s\n","END OF DEFINITION" DEND
    goto Koniec;/* Bez zliczania bo bez zagniezdzania - nie uznaje popup */

default:
    fprintf(stderr, "Ignore 0x%x==%u\n", menu[licznik].u,menu[licznik].u);	/* DEBUG? */
    };
    
}while(1);

Koniec:
	DPRT "%s 0x%x\n","SERVER ANSWER: ",ret_ptr DEND
	
	Send32(MENU_ANSWER);
	assert(sizeof(ret_ptr)==sizeof(unsigned int));
	
	Send32((unsigned)ret_ptr);	/* Na INT32 do wyslania */
	DPRT "\n" DEND
}

menu_item* root_menu=NULL;

/* INPUT INTERPRETATION                                     */
void get_file_input( XtPointer client_data,int* fid,XtInputId* id)
/************************************************************/
{
	static FILE* filek=NULL;
	Widget item;
	Widget parent = (Widget) client_data;
	menu_optcode* buffer=NULL;
	
	if(filek==NULL)
		{
		filek=fdopen(*fid, "r");
		if(filek==NULL)
		fatal("fdopen(input,\"r\")");
		}
	
	buffer = read_menu_optcodes(filek);
	
	assert(buffer!=NULL);
	
	DPRT "MOD. PKC: " DEND
	if(ssh_menu_trace)
		save_menu_optcodes(stderr,buffer); 		    /* DEBUG */
	DPRT "******************************************************************\n" DEND
	
	if(buffer->u==END_OF_CODES)
	    {
	    destroy_menu_optcodes(buffer);
	    printf("!!! sshmenux11: End of input. Bye!!!\n");
	    exit(0);
	    }
	
	/* save_menu_optcodes(stderr,buffer); DEBUG*/
	
	assert(root_menu!=NULL);
	
        modify_menu(root_menu,buffer);
	
	destroy_menu_optcodes(buffer);
	
	update_menu_tree(root_menu,NULL);
}


/* CLOSING HANDLE */
void Destruction()
{
if(root_menu)
    {
    DPRT "DESTRUCTION:\n" DEND
    free_item(root_menu);
    DPRT "DONE\n" DEND;
    Send16(0xffff);/* Potwierdza koniec komunikacji */
    root_menu=NULL;
    }
}

/*  P R O G R A M    E N T R Y  */
int main(int argc,char ** argv)
/* **************************** */
{
menu_optcode* mainMenuCode=NULL;/* MAIN MENU CODE */


    XtAppContext appContext;
    Widget topLevel, mainWindow, menuBar, frame;
    Widget fileButton, fileMenu, quit, test, helpButton, helpMenu, help, helpBox;
    Widget temp;

   /* DECODE TARGET WINDOW ID */
   if(argc>1)
   	{	
   	Target=strtoul(argv[1],NULL, 0);
   	DPRT "Target window(%s) for menu is 0x%lx == %lu \n", argv[1], Target, Target DEND
	DPRT "*****************************************************************\n"    DEND    
   	}
   	else 
   	{
   	fprintf(stderr,"What window is target?\n");
   	exit('?');
   	}

    if(argc>2)
    	{
    	ssh_menu_trace=strtoul(argv[2],NULL, 0);
    	DPRT "MENU TRACING IS ON!\n" DEND
    	}

    /* REGISTER EXIT HANDLER */
    atexit(Destruction);
		  
    /* REGISTER THE DEFAULT LANGUAGE PROCEDURE */
    XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);

    /* CREATE MAIN APLICATION WIDGET  */
    topLevel = XtVaAppInitialize(
            &appContext,        /* Application context */
            "XMainWindow",      /* application class name */
            NULL, 0,            /* command line option list */
            &argc, argv,        /* command line args */
            NULL,               /* for missing app-defaults file */
            NULL);              /* terminate varargs list */
    
    /* CREATE MENU TREE */          
    mainMenuCode=read_menu_optcodes(stdin);

    root_menu=compose_menu(mainMenuCode, NULL);
    
    destroy_menu_optcodes(mainMenuCode);
    
    if( ssh_menu_trace )  /* DEBUG */
    		print_item(stderr, root_menu,0); 
		
#ifdef MOTIF
    /* REGISTER CONVERTER FOR TEAROFF MENUS */
    XmRepTypeInstallTearOffModelConverter();
    
    /* CREATE BAR */
    menuBar = XmCreateMenuBar(
	    topLevel, 	/* parent 	*/
            "menuBar",  /* widget name 	*/
            NULL,       /* no arguments needed */
            0);         /* no arguments needed */
    
#else
    
    /* CREATE ATHENA BOX WIDGET */
    menuBar = XtCreateManagedWidget(
        "menuBox",  	/* widget name */
        boxWidgetClass, /* widget class */
        topLevel,   	/* parent widget*/
        NULL,   	/* argument list*/
        0   		/* arglist size */
        );
    
    mark = XCreateBitmapFromData(XtDisplay(topLevel),
            RootWindowOfScreen(XtScreen(topLevel)),
            xlogo16_bits, xlogo16_width, xlogo16_height);
	    
#endif
    
    /*XtManageChild(menuBar);*/
    
    update_menu_tree(root_menu, menuBar);
   
    /* CALBACK FOR EXIT */
    XtAddCallback(topLevel,XtNdestroyCallback, ForExitTopLevel, NULL);
    
    /* MAKE MENU VISIBLE AND SAVE "MyDisplay" */
    XtRealizeWidget(topLevel);
    MyDisplay=XtDisplay(topLevel);/* Musi byc na tym samym displayu co klient */
    
    /* REGISTER FUNCTION TO HANDLE THAT INPUT */
    XtAppAddInput(appContext, 
                  fileno(stdin), 
		  (XtPointer)XtInputReadMask, 
		  get_file_input, 
		  (XtPointer)NULL );

    /* STARTING MAIN LOOP */
    XtAppMainLoop(appContext);
    
    free_item(root_menu);
    
    fprintf(stderr,"sshmenux11: Suprise!!! End of main reached!\n"); /* DEBUG */
    
    return 0;
}

