/*
Implementacja najprostrzego interface'u wizualizacyjnego
dla MS Windows 16/32. Dla Borland 5.0 i MS Visual C++ 4.0
*/
#include "platform.h"
#include <windows.h>
#include <windowsx.h>
#include <signal.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#if defined( __WIN16__ ) 
#include "win16ext.h"           // required only for win16 applications
#endif
#include "symshwin.h"            // prototypes specific to this application
#define  MY_WIN_STYLE		      (WS_OVERLAPPEDWINDOW/* | WS_HSCROLL | WS_VSCROLL*/)
#include "symshell.h"

#define MINUSERCOMMAND  IDM_EXIT 

/* For close_plot() */
extern int WB_error_enter_before_clean;
 
static char szAppName[14]="SYMSHELL";   // The name of this application
static HANDLE hAccelTable;
HINSTANCE WB_Instance=0;
HINSTANCE WB_PrevInstance=0;
HWND	  WB_Hwnd=0;	
static HWND MyHwnd=0;						  // Main window handle.

static HDC MyHdc=0;					//Current HDC
static HDC WnHdc=0;					//Window HDC
static HDC MbHdc=0;					//Memory bitmap HDC

static HBITMAP VirtualScreen=0;		//Memory bitmap CreateCompatible BitBlt
static COLORREF colors[256]; 
static HBRUSH   brushes[256];
static HPEN		pens[256];

static char* progname="WB SYMSHELL APPLICATION "__DATE__;
static int curr_color=-1;
static int curr_fill=-1;
static int background=0;
static int is_mouse=0;
static int animate=0;
static int buffered=0;
static int trace=0;
static char* window_name="Windows WB SYMSHELL interface "__DATE__;

static int W_width,W_height;
static int mulx=1;
static int muly=1;
static int ini_width;//MAXX+1 obszaru bitowego
static int ini_height;//MAXY+1 obszaru bitowego
static int ini_col;//Ile kolumn tekstu dodatkowo
static int ini_row;//Ile wierszy tekstu dodatkowo
static int Is_finfo=0;
static TEXTMETRIC font_info;
static MSG msg;
static int WindowClosed=1;// WIndow destroyed/notopen flag for close_plot
static int CharToGet=0;
static char* icon_name="Windows SYMSHELL "__DATE__;
static int UseGrayScale=0;//Flaga uzycia skali szarosci

#ifdef INCLUDE_AUTOR
#include "autor.h"
extern char* SymShellProd="Windows Symshell . Compiled "__DATE__ __TIME__;
extern char* Copyright="Copyright © 1997 for this program Wojciech Borkowski from Warsaw University";
extern void symshell_about()
{
wb_about(window_name);
}
#endif


static int error_proc(void)
{
DWORD code=GetLastError();
return 'A';// abort();
}

static HBRUSH GetMyBrush(int color)
{
if(brushes[color]==0)//Trtzeba alokowac pedzel
	brushes[color]=CreateSolidBrush(colors[color]);
return brushes[color];
}

static HPEN GetMyPen(int color)
{
if(pens[color]==0)//Trztzeba alokowac pedzel
	{
	int pom=mulx<muly?mulx:muly;
	pens[color]=CreatePen(PS_SOLID,pom,colors[color]);
	}
return pens[color];
}

/*
static HPEN Get1Pen(int color)
{
if(pens[color]==0)//Trztzeba alokowac pedzel
	{
	pens[color]=CreatePen(PS_SOLID,1,colors[color]);
	}
return pens[color];
}
*/

static HDC GetRealScreen(void)
{
if(MyHdc==0)	
	{
	MyHdc=GetDC(MyHwnd);
	GetTextMetrics(MyHdc,&font_info);
	Is_finfo=1;
	}
return MyHdc;
}

static HDC GetVirtualScreen(void)
{	
if(MbHdc==0)
	{
	RECT rc;
	HDC WindowDC=GetDC(MyHwnd);
	MbHdc=CreateCompatibleDC(WindowDC);
	VirtualScreen=CreateCompatibleBitmap(WindowDC,W_width*mulx,W_height*muly);
	SelectObject(MbHdc,VirtualScreen);
	GetClientRect(MyHwnd, &rc);
	FillRect(MbHdc, &rc, GetMyBrush(background));
	ReleaseDC(MyHwnd,WindowDC);
	GetTextMetrics(MbHdc,&font_info);
	Is_finfo=1;
	}
return MbHdc;
}

HDC GetMyHdc(void)
/* DC caching */
{
if(MyHdc==0)	
      if(animate || buffered)
		{
		return MyHdc=GetVirtualScreen();
		}
		else
		{
		return GetRealScreen();
		}
return MyHdc;
}

static void FreeResources(void)
{
int k;
curr_color=curr_fill=-1;

if(trace)
	fprintf(stderr," FREE RESORCES ");

if(MbHdc)		  //Zwalnia wirtualny kontekst jesli jest
	{ if(MbHdc==MyHdc) MyHdc=0; DeleteDC(MbHdc); MbHdc=0;}

if(VirtualScreen) //Zwalnia ekran wirtualny jesli jest
	{ DeleteObject(VirtualScreen); VirtualScreen=0; }

if(MyHdc!=0)		//Zwalnia kontekst okna jesli jest
	{ ReleaseDC(MyHwnd,MyHdc); MyHdc=0;   }

for(k=0;k<256;k++)
	{
   if(pens[k]!=0)
		{DeleteObject(pens[k]);pens[k]=0; }
   if(brushes[k]!=0)
		{DeleteObject(brushes[k]);brushes[k]=0; }
   }
}

static void SetScale(void)
{
#ifndef M_PI
const double M_PI=3.141595;
#endif
extern void set_rgb(int color,int r,int g,int b);
int k;
if(UseGrayScale)
  {
  for(k=0;k<255;k++)
    { 
    long wal=k;
    /*printf("%u %ul\n",k,wal);*/
    set_rgb(k,wal,wal,wal);
    }      
  }
  else
  {	  
  for(k=0;k<255;k++)
	{
	long wal1,wal2,wal3;
	double kat=(M_PI*2)*k/255.;
	wal1=(long)(255*sin(kat*1.25));
	if(wal1<0) wal1=0;
	wal2=(long)(255*(-sin(kat*0.85)));
	if(wal2<0) wal2=0;
	wal3=(long)(255*(-cos(kat*1.1)));
	if(wal3<0) wal3=0;
	set_rgb(k,wal1,wal2,wal3);
	}
  }
set_rgb(255,255,255,255);
if(trace)
	printf("%s\n","SetScale completed");
}



static int InitWindowClass(void)
{
// Other instances of app running?
if (!WB_PrevInstance)
    {
        // Initialize shared things
        if (!InitApplication(WB_Instance))
        {
            return FALSE;               // Exits if unable to initialize
        }
    }
return TRUE;
}

static int InitMainWindow(void)
{
// Perform initializations that apply to a specific instance
if (!InitInstance(WB_Instance))
    {
        return FALSE;
    }
hAccelTable = LoadAccelerators(WB_Instance, szAppName);
return TRUE;
}


/* CONFIGURATION AND MENAGE SHELL - USE IN THIS ORDER! */
int  mouse_activity(int yes)
/* Ustala czy mysz ma byc obslugiwana. Zwraca stan flagi */
{
int pom=is_mouse;
is_mouse=yes;
if(MyHwnd!=0)
	{
   if(is_mouse)
	   SetCursor(LoadCursor(NULL, IDC_ARROW));
      else
      SetCursor(LoadCursor(NULL, IDC_NO));
   }
return pom;
}

void set_background(unsigned char c)
/* Ustala index koloru do czyszczenia itp */
{
background=c;
}

unsigned get_background()
{
return background;
}

void buffering_setup(int _n)
/* Przelaczanie buforowanie okna na serverze */
{
if(_n)
	animate=1;
	else
	animate=0;
if(animate)	/* Musi byc wlaczona bitmapa buforujaca */
	buffered=1;/* zeby mozna bylo na nia pisac */
}


void shell_setup(char* title,int iargc,char* iargv[])
/* Przekazanie parametrow wywolania */
{
/* Przekazanie parametrow wywolania */
int i;
int largc=iargc;
char** largv=iargv;
progname=largv[0];

for(i=1;i<largc;i++)
	{
#ifdef INCLUDE_AUTOR
	if(strcmp(largv[i],"-logo")==0)
	{
	wb_about(icon_name);
	}
	else
#endif
	if(strncmp(largv[i],"-h",2)==0)
		{
		printf("SYMSHELL for MS Windows. Compiled %s\n",__DATE__);
		printf("Supported swithes:\n\t -mapped[+/-] \n\t -buffered[+/-]"
				"\n\t -traceenv[+/-] \n\t -gray[+/-] \n"); 
		}
	else
	if(strncmp(largv[i],"-gray",5)==0)
		{
		UseGrayScale=(largv[i][5]=='+')?1:0;
	        printf("Gray scale is %s\n",(UseGrayScale?"ON":"OFF"));
		}
	else
	if(strncmp(largv[i],"-mapped",7)==0)
		{
		buffered=(largv[i][7]=='+')?1:0;
	        printf("Double mapping is %s\n",(buffered?"ON":"OFF"));
		if(!buffered)	/* Jak nie ma bitmapy to nie mozna */
			animate=0; /* animowac */
		} 
	else
	if(strncmp(largv[i],"-mouse",6)==0)
		{
		is_mouse=(largv[i][6]=='+')?1:0;
		printf("Mouse is %s\n",(is_mouse?"ON":"OFF"));
		}
	else
	if(strncmp(largv[i],"-buffered",9)==0)
		{
		animate=(largv[i][9]=='+')?1:0;
	        printf("Buffered is %s\n",(buffered?"ON":"OFF"));
		//if(animate)	/* Musi byc wlaczona bitmapa buforujaca */
		//	buffered=1;/* zeby mozna bylo na nia pisac */
			buffered=animate;/*Albo,albo.NIe jak w Xwindow */
		}
   /*
	else
	if(strncmp(largv[i],"-bestfont",9)==0)
		{
		ResizeFont=(largv[i][9]=='+')?1:0;
	        printf("Search best font is %s\n",(ResizeFont?"ON":"OFF"));
		}
   */
	else
	if(strncmp(largv[i],"-traceevt",9)==0)
		{
		trace=(largv[i][9]=='+')?1:0;
	        printf("Trace events is %s\n",(trace?"ON":"OFF"));
		}
	}
if(animate)	/* Musi byc wlaczona bitmapa buforujaca */
	buffered=1;/* zeby mozna bylo na nia pisac */
window_name = title;
icon_name = title;
}

/* GETTING SCREEN PARAMETERS */
/*---------------------------*/
int  screen_height()
/*ostateczne rozmiary okna po przeliczeniach z init_plot*/
{
return W_height;
}

int  screen_width()
/*do pozycjonowania na marginesach */
{
return W_width;
}

static int  _char_height()
/* Aktualne rozmiary znaku  */
{
if(!Is_finfo)
	GetMyHdc();//Wymusza przeczytanie informacji o foncie
return  font_info.tmHeight;
}

static int  _char_width()
/* potrzebne do pozycjonowania tekstu */
{
if(!Is_finfo)
	GetMyHdc();//Wymusza przeczytanie informacji o foncie
return  font_info.tmAveCharWidth;
}

int  char_height(char znak)
/* Aktualne rozmiary znaku  */
{
int pom=_char_height()/muly;
return pom;
}

int  char_width(char znak)
/* potrzebne do pozycjonowania tekstu */
{
int pom=_char_width()/mulx;
return pom;
}

int  string_height(const char* str)
/* Aktualne rozmiary lancucha */
{
SIZE sizes;
if(GetTextExtentPoint32(GetMyHdc(),str,strlen(str),&sizes))
		return sizes.cy/muly;
		else
		return -1;
}

int  string_width(const char* str)
/* ...potrzebne do jego pozycjonowania */
{
SIZE sizes;
if(GetTextExtentPoint32(GetMyHdc(),str,strlen(str),&sizes))
		return sizes.cx/mulx;
		else
		return -1;
}

int init_plot(int a,int b,int ca, int cb)
/* typowa dla platformy inicjacja grafiki/semigrafiki */
/* a,b to wymagane rozmiary ekranu w pixelach */
/* ca,cb - ile dodatkowych lini i kolumn na tekst (w znakach!) */
{
RECT rect;
#ifdef __BORLANDC__
extern HINSTANCE _hInstance; //z c0.obj
if(WB_Instance==0)
            WB_Instance=_hInstance;
#else
if(WB_Instance==0)
			WB_Instance=GetModuleHandle(NULL);
#endif
ini_width=a;//MAXX+1 obszaru bitowego
ini_height=b;//MAXY+1 obszaru bitowego
ini_col=ca;//Ile kolumn tekstu dodatkowo
ini_row=cb;//Ile wierszy tekstu dodatkowo
SetScale();
W_width=a;W_height=b;// Zeby cos bylo
if(!InitWindowClass()||
	!InitMainWindow())
	{
	assert("Window initialisation"==NULL);
   	return /*FALSE*/0;
	}

atexit(close_plot);
// Resize
//ShowWindow(MyHwnd, SW_SHOW); // Show the window

GetRealScreen();//Laduje niezbedna informacje o czcionce
W_width=a+ca*_char_width();
W_height=b+cb*_char_height();
rect.left=rect.top=100;
rect.right=W_width;
rect.bottom=W_height;
AdjustWindowRect(&rect,MY_WIN_STYLE,1);
SetWindowPos(  MyHwnd,
					HWND_TOP,0,0,
					(100-rect.left)+rect.right,(100-rect.top)+rect.bottom,
					SWP_NOMOVE | SWP_NOCOPYBITS);
// Make the window visible; update its client area
ShowWindow(MyHwnd, SW_SHOW); // Show the window
WindowClosed=0;// WIndow open flag for close_plot
//if(!is_mouse)
//      SetCursor(LoadCursor(NULL, IDC_NO));
FreeResources(); //Bo moga sie zaladowac konteksty dla zlego ekranu

PostMessage(MyHwnd,WM_PAINT,0,0); // Sends WM_PAINT message
if(is_mouse)
      SetCursor(LoadCursor(NULL, IDC_ARROW));
      else
      SetCursor(LoadCursor(NULL, IDC_NO));
return /*TRUE*/1;
}

void flush_plot()
/* uzgodnienie zawartosci ekranu  */
{
if(trace)
	 fprintf(stderr,"FLUSH PLOT ");

if(MyHdc!=0)//Byl uzywany. Zwalnia tylko RelaseResources
	{
	if(animate && VirtualScreen && MyHdc==MbHdc )//Animujemy i jest juz bitmapa
		{
		HDC ScreenDc=GetDC(MyHwnd);
		if(trace)
			fprintf(stderr,"-> BITBLT ");
		BitBlt(ScreenDc,0,0,W_width*mulx,W_height*muly,MyHdc,0,0,SRCCOPY);
		ReleaseDC(MyHwnd,ScreenDc);
		}
	if(trace)
			fprintf(stderr,"-> RELEASE DC ");
	ReleaseDC(MyHwnd,MyHdc);
	curr_color=curr_fill=-1;
	MyHdc=0;//!!!
   }

if(trace)
	fprintf(stderr,"\n");
}


/* GETTING INPUT */
//zmienne do komunikacji ProcesMsg z procedurami obslugi komunikatow
static int InputChar=0;
static int InputXpos;
static int InputYpos;
static int InputClick;
static int InputFlag=0;

static int ProcessMsg(int peek)
//Przerabia wejscie i zwraca znak:
// EOF jesli koniec, 0 jesli nic do wziecia lub inny
{
BOOL msg_avail=0;
int ret;
InputChar='\0';
// Acquire and dispatch messages until a WM_QUIT message is received.
if(peek)
	  {
	  msg_avail=PeekMessage(&msg,MyHwnd,0,0,PM_REMOVE);
      }
      else
      {
      msg_avail=GetMessage(&msg,MyHwnd, 0, 0);
		if(msg_avail==FALSE)
				{WindowClosed=1;// WIndow destroye
             InputChar='\0';
				return EOF;}
      }
if(msg_avail==-1)
	if(error_proc()=='A') abort();
if(msg_avail==TRUE)
  if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
   {
   TranslateMessage(&msg);
   DispatchMessage(&msg);
   }
ret=InputChar;
InputChar='\0';
return ret;
}

static int first_to_read=0;

int  input_ready()
/* (nie)zalezna od platformy funkcja sprawdzajaca czy jest wejscie */
{
int input;
if(CharToGet!=0||first_to_read!=0)//Nieodebrano
		return TRUE;
input=ProcessMsg(1);//PEEK! Trzeba sprawdzic nowe komunikaty
if(input!='\0')
	{CharToGet=input;return TRUE;}
   else
	{CharToGet='\0';return FALSE;}
}

int  set_char(int c)	
/* Odeslanie znaku na wejscie - zwraca 0 jesli nie ma miejsca */
{
if(first_to_read!=0)//Nieodebrano
		return 0;
first_to_read=c;
return 1;
}

int  get_char()
/* odczytywanie znakow sterowania, z oczekiwaniem jesli brak */
{
int input;
if(first_to_read!=0) //Sprawdza czy nie ma zwrotow
	{
	input=first_to_read;
	first_to_read=0;
	return input;
	}
if(CharToGet!=0)//Zwraca znak odczytany przez input_ready()
   {
   input=CharToGet;
   CharToGet='\0';
   return input;
   }
while((input=ProcessMsg(0))=='\0');//sam musi poczekac na znak
return input;
}

int  get_mouse_event(int* xpos,int* ypos,int* click)
/* Odczytuje ostatnie zdazenie myszy */
{
if(InputFlag!=0)
	{
	*xpos=InputXpos/mulx;
	*ypos=InputYpos/muly;
	*click=InputClick;
    InputFlag=0;
    return 1;
	}
return 0;
}

void close_plot(void)
/* zamkniecie grafiki/semigrafiki */
{
if(!WindowClosed) // Jeszcze nie zamkniete innym sposobem
	{
    if(WB_error_enter_before_clean)
		{
		char* kom="Press OK to close the window";
		/* width,height of Window at this moment */
		//print(ini_width/2-(strlen(kom)*char_width('X'))/2,ini_height/2,kom);
		fprintf(stderr,"(See at window: %s )",window_name);
		MessageBox(MyHwnd,kom,window_name,MB_ICONQUESTION);
		//get_char();
		WB_error_enter_before_clean=0;
		}
   FreeResources();
   DestroyWindow(MyHwnd);
   WindowClosed=1; //Gdyby wywoloano powtornie ta funkcje
   // Message loop for ending meesages - should be?(WB)
   while(GetMessage(&msg,NULL, 0, 0)!=FALSE)
   	{
   	if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
	   	{
	   	TranslateMessage(&msg);
		   DispatchMessage(&msg);
   		}
   	}
   }
}



/* PRINTING */
static char straznik1=0x77;
static char bufor[1024];
static char straznik2=0x77;
static int ox,oy;

void print(int x,int y,char* format,...)
/* ---//--- wyprowadzenie tekstu na ekran */
{
HDC MyHdc;
RECT rc;
size_t len=0;
unsigned font_height=0;
unsigned font_width=0;

   va_list argptr;

   va_start(argptr, format);

   vsprintf(bufor, format, argptr);

   va_end(argptr);

   if(straznik1!=0x77 || straznik2!=0x77)
		{
		fprintf(stderr,"symshell.print(...) - line exced 1024b!");
		abort();
		}

  /* Print string in window */
  /* Need length for both XTextWidth and XDrawString */
    len = strlen(bufor);

    /* Get string widths & hight */
    MyHdc=GetMyHdc();
    font_height = font_info.tmHeight;
    font_width = font_info.tmAveCharWidth;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplicaton of coordinates */
    y*=muly; /* if window is bigger */

//    ox=x;oy=y;
	rc.left=x;rc.top=y;
	rc.right=x+font_width*(len+1);
	rc.bottom=y+font_height;
	//FillRect (MyHdc, &rc, GetMyBrush(255));

    SetTextColor(MyHdc,colors[0]);
    SetBkColor(MyHdc,colors[255]);

//if(!animate)
    TextOut(MyHdc,x,y,bufor,len);
//if(buffered)
//    TextOut(MyBtmHdc,x,y,bufor,len);
}

void printc(int x,int y,
    unsigned char fore,unsigned char back,
    char* format,...)
{
HDC MyHdc;
RECT rc;
size_t len=0;
unsigned font_height=0;
unsigned font_width=0;

   va_list argptr;

   va_start(argptr, format);

   vsprintf(bufor, format, argptr);

   va_end(argptr);

   if(straznik1!=0x77 || straznik2!=0x77)
		{
		fprintf(stderr,"symshell.print(...) - line exced 1024b!");
		abort();
		}

  /* Need length for FillRect*/
    len = strlen(bufor);

    /* Get string widths & hight */
    MyHdc=GetMyHdc();
    font_height = font_info.tmHeight;
    font_width = font_info.tmAveCharWidth;

    /* Output text, centered on each line */

    x*=mulx; /* Multiplicaton of coordinates */
    y*=muly; /* if window is bigger */

//    ox=x;oy=y;
	rc.left=x;rc.top=y;
	rc.right=x+font_width*(len+1);
	rc.bottom=y+font_height;
	//FillRect (MyHdc, &rc, GetMyBrush(back));

	 SetTextColor(MyHdc,colors[fore]);
    SetBkColor(MyHdc,colors[back]);
//if(!animate)
    TextOut(MyHdc,x,y,bufor,len);
//if(buffered)
//    TextOut(MyBtmHdc,x,y,bufor,len);
}



/* DRAWING  */
void plot(int x,int y,unsigned  char c)
/* wyswietlenie punktu na ekranie */
{
x*=mulx; /* Multiplicaton of coordinates */
y*=muly; /* if window is bigger */
if(mulx>1 || muly>1)
   {
   RECT rc;
   rc.left=x;rc.top=y;
   rc.right=x+mulx;
   rc.bottom=y+muly;
	FillRect (GetMyHdc(), &rc, GetMyBrush(c));
   }
   else
	SetPixelV(GetMyHdc(),x,y,colors[c]);
}

void fill_rect(int x1,int y1,int x2,int y2,unsigned  char c)
{
RECT rc;
x1*=mulx;x2*=mulx; /* Multiplicaton of coordinates */
y1*=muly;y2*=muly;  /* if window is bigger */
rc.left=x1;rc.top=y1;
rc.right=x2;
rc.bottom=y2;
FillRect (GetMyHdc(), &rc, GetMyBrush(c));
}


void line(int x1,int y1,int x2,int y2,unsigned  char c)
/* wyswietlenie lini */
{
POINT points[2];
plot(x2,y2,c);
plot(x1,y1,c);

x1*=mulx;x2*=mulx; /* Multiplicaton of coordinates */
y1*=muly;y2*=muly;  /* if window is bigger */

if(curr_color!=c)
	{
   HPEN MyPen=GetMyPen(c);
   SelectObject(GetMyHdc(),MyPen);
   }
points[0].x=x1;points[0].y=y1;
points[1].x=x2;points[1].y=y2;
Polyline(GetMyHdc(),points,2);
}

void circle(int x,int y,int r,unsigned char c)
/* Wyswietlenie okregu w kolorze c */
{
int r1,r2;
x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
r1=r*mulx;r2=r*muly;  /* if window is bigger */
if(curr_color!=c)
	{
   HPEN MyPen=GetMyPen(c);
   SelectObject(GetMyHdc(),MyPen);
   }
Arc(GetMyHdc(),x-r1,y-r1,x+r2,y+r2,x,y+r1,x,y+r1);
}

void fill_circle(int x,int y,int r,unsigned char c)
/* KOlo w kolorze c */
{
int r1,r2;
x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
r1=r*mulx;r2=r*muly;  /* if window is bigger */
if(curr_color!=c)
	{
	HPEN MyPen=GetMyPen(c);
	SelectObject(GetMyHdc(),MyPen);
	}
if(curr_fill!=c)
	{
	HBRUSH MyBrush=GetMyBrush(c);
	SelectObject(GetMyHdc(),MyBrush);
	}
Ellipse(GetMyHdc(),x-r1,y-r2,x+r1,y+r2);
}

void fill_poly(int vx,int vy,
					const ssh_point points[],int number,
					unsigned  char c)
/* Wypelnia wielokat przesuniety o vx,vy w kolorze c */
{
static POINT _LocalTable[10];
POINT* LocalPoints=_LocalTable;
int i;

if(number<=2)
		return; //Nie da sie rysowac wielokata o dwu punktach lub mniej

if(number>10) //Jest za duzy.Alokacja
	LocalPoints=calloc(number,sizeof(POINT));

if(LocalPoints==NULL)
		 {assert("Memory for polygon");return;}

if(curr_color!=c)
	{
	HPEN MyPen=GetMyPen(c);
	SelectObject(GetMyHdc(),MyPen);
	}

if(curr_fill!=c)
	{
	HBRUSH MyBrush=GetMyBrush(c);
	SelectObject(GetMyHdc(),MyBrush);
	}

vx*=mulx;
vy*=muly;
for(i=0;i<number;i++)
	{
	LocalPoints[i].x=points[i].x*mulx+vx;
	LocalPoints[i].y=points[i].y*muly+vy;
	}

Polygon(GetMyHdc(),LocalPoints,number);

if(number>10) //Byl duzy
	free(LocalPoints);
}

void clear_screen()
/* Czysci ekran przed zmiana zawartosci */
{
RECT rc;
if(trace)
	fprintf(stderr,"CLEAR SCREEN\n");
GetClientRect(MyHwnd, &rc);
FillRect (GetMyHdc(), &rc, GetMyBrush(background));
}

void set_rgb(int color,int r,int g,int b)
/* Zmienia definicja koloru. Indeksy 0..255 */
{
colors[color]=RGB(r,g,b);
if(brushes[color]!=0)
		{
      if(curr_fill==color)
      	   {
      		curr_fill=-1;/* Wymusza wymiane pedzla przy nastepnym rysowaniu*/
            if(MyHdc!=0)
	            SelectObject(MyHdc,GetStockObject(NULL_BRUSH));/*Wymiata z kontekstu */
            }
      DeleteObject(brushes[color]);/* Teraz mozna juz zwolnic */
		brushes[color]=0;
      }
if(pens[color]!=0)
		{
      if(curr_color==color)
      		{
      		curr_color=-1;/* Wymusza wymiane piora przy nastepnym rysowaniu*/
            if(MyHdc!=0)
            	SelectObject(MyHdc,GetStockObject(NULL_PEN));/*Wymiata z kontekstu */
            }
      DeleteObject(pens[color]);/* Teraz mozna juz zwolnic */
		pens[color]=0;
		}
}


// MESSAGE SUPPORT COPIED FROM BORLAND SAMPLE
LRESULT DispDefault(EDWP, HWND, UINT, WPARAM, LPARAM);

//
//  FUNCTION: DispMessage(LPMSDI, HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Call the function associated with a message.
//
//  PARAMETERS:
//    lpmsdi - Structure containing the message dispatch information.
//    hwnd - The window handle
//    uMessage - The message number
//    wparam - Message specific data
//    lparam - Message specific data
//
//  RETURN VALUE:
//    The value returned by the message function that was called.
//
//  COMMENTS:
//    Runs the table of messages stored in lpmsdi->rgmsd searching
//    for a message number that matches uMessage.  If a match is found,
//    call the associated function.  Otherwise, call DispDefault to
//    call the default function, if any, associated with the message
//    structure.  In either case, return the value recieved from the
//    message or default function.
//

LRESULT DispMessage(LPMSDI lpmsdi,
                    HWND   hwnd, 
                    UINT   uMessage,
                    WPARAM wparam, 
                    LPARAM lparam)
{
    int  imsd;

    MSD *rgmsd = lpmsdi->rgmsd;
    int  cmsd  = lpmsdi->cmsd;

    for (imsd = 0; imsd < cmsd; imsd++)
    {
        if (rgmsd[imsd].uMessage == uMessage)
            return rgmsd[imsd].pfnmsg(hwnd, uMessage, wparam, lparam);
    }

    return DispDefault(lpmsdi->edwp, hwnd, uMessage, wparam, lparam);
}


//
//  FUNCTION: DispDefault(EDWP, HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Call the appropriate default window procedure.
//
//  PARAMETERS:
//    edwp - Enumerate specifying the appropriate default winow procedure.
//    hwnd - The window handle
//    uMessage - The message number
//    wparam - Message specific data
//    lparam - Message specific data
//
//  RETURN VALUE:
//    If there is a default proc, return the value returned by the
//    default proc.  Otherwise, return 0.
//
//  COMMENTS:
//    Calls the default procedure associated with edwp using the specified
//    parameters.
//

LRESULT DispDefault(EDWP   edwp,
                    HWND   hwnd,
                    UINT   uMessage,
                    WPARAM wparam,
                    LPARAM lparam)
{
    switch (edwp)
    {
        case edwpNone:
            return 0;
        case edwpWindow:
            return DefWindowProc(hwnd, uMessage, wparam, lparam);
        case edwpDialog:
            return DefDlgProc(hwnd, uMessage, wparam, lparam);
        case edwpMDIFrame:
            return DefFrameProc(hwnd, hwndMDIClient, uMessage, wparam, lparam);
        case edwpMDIChild:
            return DefMDIChildProc(hwnd, uMessage, wparam, lparam);
    }
    return 0;
}



static BOOL InitApplication(HINSTANCE hInstance)
{
    #ifdef __WIN16__
    WNDCLASS  wc;
    #else
    WNDCLASSEX wc;
    #endif

    // Load the application name and description strings.

    LoadString(hInstance, IDS_APPNAME, szAppName, sizeof(szAppName));
//    LoadString(hInstance, IDS_DESCRIPTION, szTitle, sizeof(szTitle));

    // Fill in window class structure with parameters that describe the
    // main window.

    #ifndef __WIN16__
    wc.cbSize        = sizeof(WNDCLASSEX);
	wc.hIconSm       = LoadImage(hInstance,		// Load small icon image
	                             MAKEINTRESOURCE(IDI_APPICON),
	                             IMAGE_ICON,
	                             16, 16,
	                             0);
    #endif
    wc.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // Class style(s).
    wc.lpfnWndProc   = (WNDPROC)WndProc;        // Window Procedure
    wc.cbClsExtra    = 0;                       // No per-class extra data.
    wc.cbWndExtra    = 0;                       // No per-window extra data.
    wc.hInstance     = hInstance;               // Owner of this class
    wc.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPICON)); // Icon name from .RC
    wc.hCursor       = is_mouse!=0?
							LoadCursor(NULL, IDC_ARROW):
							LoadCursor(NULL, IDC_NO); // Cursor
    wc.hbrBackground = //(HBRUSH)(COLOR_WINDOW + 1); // Default color
    							CreateSolidBrush(colors[background]);
    wc.lpszMenuName  = szAppName;               // Menu name from .RC
    wc.lpszClassName = szAppName;               // Name to register as

    // Register the window class and return FALSE if unsuccesful.

    #ifdef __WIN16__
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    #else
    if (!RegisterClassEx(&wc))
    {
        if( !RegisterClass((LPWNDCLASS)&wc.style) )
		{
		DWORD err=GetLastError();
		char buf[256];
		sprintf(buf,"Can't registry window class! code:%ld",err);
		MessageBox(0,buf,"Unexpected error",MB_ICONSTOP);
        return FALSE;
		}
    }
    #endif
    //
    // **TODO** Call module specific application initialization functions here.
    //

    return TRUE;
}


static BOOL InitInstance(HINSTANCE hInstance)
{

    // Create a main window for this application instance.
    MyHwnd = CreateWindow(szAppName,           // See RegisterClass() call.
                        window_name,           // Text for window title bar.
                        MY_WIN_STYLE,
                        CW_USEDEFAULT, 0,    // Use default positioning
                        W_width,W_height,    // Use user defined size
                        NULL,                // Overlapped has no parent.
                        NULL,                // Use the window class menu.
                        hInstance,
                        NULL);

    // If window could not be created, return "failure"
    if (!MyHwnd)
        return FALSE;

	WB_Hwnd=MyHwnd; //Save to global variable

    //
    // **TODO** Call module specific instance initialization functions here.
    //

    // **INPUT** Initialize the input module.
    if (!InitInput(MyHwnd))
    {
        return FALSE;
    }

    return TRUE;                // We succeeded...
}

// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: input.c
//
//  PURPOSE: Show windows input: mouse, keyboard, control(scroll), and timer
//
//  FUNCTIONS:
//    WndProc - Processes messages for the main window.
//    MsgCommand - Handle the WM_COMMAND messages for the main window.
//    MsgCreate - Set the timer for five-second intervals.
//    MsgDestroy - Kills the timer and posts the quit message.
//    MsgMouseMove - Display mouse move message and its parameters.
//    MsgLButtonDown -
//      Display left mouse button down message and its parameters.
//    MsgLButtonUp - Display left mouse button up message and its parameters.
//    MsgLButtonDoubleClick -
//      Display left mouse button double click message and its parameters.
//    MsgRButtonDown -
//      Display right mouse button down message and its parameters.
//    MsgRButtonUp - Display right mouse button up message and its parameters.
//    MsgRButtonDoubleClick -
//      Display right mouse button double click message and its parameters.
//    MsgKeyDown - Display key down message and its parameters.
//    MsgKeyUp - Display key up message and its parameters.
//    MsgChar - Display character recieved message and its parameters.
//    MsgTimer - Display timer message and a current time.
//    MsgScroll - Display scrollbar events and position.
//    MsgPaint - Draw the strings for current messages.
//    InitInput - Set up the rectangles for dispay of each type of message.
//
//   COMMENTS:
//    Message dispatch table -
//      For every message to be handled by the main window procedure
//      place the message number and handler function pointer in
//      rgmsd (the message dispatch table).  Place the prototype
//      for the function in globals.h and the definition of the
//      function in the appropriate module.
//    Globals.h Contains the definitions of the structures and dispatch.c
//      contains the functions that use these structures.
//

static UINT idTimer;                //  timer ID
static int  nTimerCount = 0;        //  current timer count

#define TIMERID ((UINT) 't')

// Main window message table definition.
MSD rgmsd[] =
{
    {WM_COMMAND,        MsgCommand},
    {WM_SYSCOMMAND,		MsgSysCommand},
    {WM_CREATE,         MsgCreate},
    {WM_CLOSE,				MsgClose},    //Send EOF to symshell
//    {WM_GETMINMAXINFO, MsgGetMinMaxInfo},
    {WM_SIZE,				MsgSize},	  //resize client area
    {WM_DESTROY,        MsgDestroy},
//    {WM_MOUSEMOVE,      MsgMouseMove},
    {WM_LBUTTONDOWN,    MsgLButtonDown},
//    {WM_LBUTTONUP,      MsgLButtonUp},
//    {WM_LBUTTONDBLCLK,  MsgLButtonDoubleClick},
    {WM_RBUTTONDOWN,    MsgRButtonDown},
//    {WM_RBUTTONUP,      MsgRButtonUp},
//    {WM_RBUTTONDBLCLK,  MsgRButtonDoubleClick},
//    {WM_KEYDOWN,        MsgKeyDown},
//    {WM_KEYUP,          MsgKeyUp},
    {WM_CHAR,           MsgChar},
//    {WM_TIMER,          MsgTimer},
//    {WM_HSCROLL,        MsgScroll},
//    {WM_VSCROLL,        MsgScroll},
    {WM_PAINT,          MsgPaint}
};

MSDI msdiMain =
{
    sizeof(rgmsd) / sizeof(MSD),
    rgmsd,
    edwpWindow
};


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  PARAMETERS:
//    hwnd     - window handle
//    uMessage - message number
//    wparam   - additional information (dependant on message number)
//    lparam   - additional information (dependant on message number)
//
//  RETURN VALUE:
//    The return value depends on the message number.  If the message
//    is implemented in the message dispatch table, the return value is
//    the value returned by the message handling function.  Otherwise,
//    the return value is the value returned by the default window procedure.
//
//  COMMENTS:
//    Call the DispMessage() function with the main window's message dispatch
//    information (msdiMain) and the message specific information.
//

LRESULT CALLBACK WndProc
    (HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    return DispMessage(&msdiMain, hwnd, uMessage, wparam, lparam);
}


//
//  FUNCTION: MsgCommand(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Handle the WM_COMMAND messages
//
//  PARAMETERS:
//    hwnd - The window handle
//    uMessage - WM_COMMAND (unused)
//    GET_WM_COMMAND_ID(wparam,lparam) - The command number
//
//  RETURN VALUE:
//    Depends on the command.
//
//  COMMENTS:
//
//

LRESULT MsgCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    LRESULT lRet = 0;
	// Processing WM_COMMAND
    // Message packing of wparam and lparam have changed for Win32,
    // so use the GET_WM_COMMAND macro to unpack the command
	WORD cmd=GET_WM_COMMAND_ID(wparam,lparam);
	if(cmd==IDM_SIGNAL_COMMAND)//signals symulation
		{
		raise(lparam);//Not HWND in this case!
		}
		else
		if(cmd>=MINUSERCOMMAND)
			{
			if(cmd==IDM_EXIT)
				InputChar=EOF; //Send EOF to symshell
				else
				InputChar=cmd; //Send cmd value to symmshell

			}
			else
				lRet = DefWindowProc(hwnd, uMessage, wparam, lparam);

    return lRet;
}

// Handler for WM_SYSCOMMAND
LRESULT MsgSysCommand(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    LRESULT lRet = 0;

    // Message packing of wparam and lparam have changed for Win32,
    // so use the GET_WM_COMMAND macro to unpack the commnad

    switch (GET_WM_COMMAND_ID(wparam,lparam))
    {
        //
        // **TODO** Add cases here for application specific command messages.
        //

        case SC_CLOSE:
				InputChar=EOF; //Send EOF to symshell
            break;
//        case SC_SIZE:
//        		InputChar='\0';
// 				USE DEFAULT SUPPORT!   don't break; or move this steatment
        default:
            lRet = DefWindowProc(hwnd, uMessage, wparam, lparam);
    }

    return lRet;
}


//
//  FUNCTION: MsgCreate(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Set the timer for five-second intervals
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_CREATE      (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Extra data     (Unused)
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#pragma argsused
LRESULT MsgCreate(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    // Set the timer for five-second intervals
//    idTimer =  SetTimer(hwnd, TIMERID, 5000, NULL);
	 if(trace)
		fprintf(stderr,"CREATE MSG\n");
	 return 0;
}


#pragma argsused
LRESULT MsgClose(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	InputChar=EOF; //Send EOF to symshell
	 return 0;
}

//
//  FUNCTION: MsgGetMinMaxInfo(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Sets minimum window size.
//
//  PARAMETERS:
//
//    hwnd      - Window handle
//    uMessage  - Message number (Unused)
//    wparam    - Extra data     (Unused)
//    lparam    - Address of MINMAXINFO structure
//
//  RETURN VALUE:
//
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#pragma argsused
LRESULT MsgGetMinMaxInfo(HWND   hwnd,
                         UINT   uMessage,
                         WPARAM wparam,
                         LPARAM lparam)
{
    static int     cxMin=0;
    static int     cyMin=0;
    MINMAXINFO FAR *lpmmi;

    lpmmi = (MINMAXINFO FAR *)lparam;
/*
	 if (cxMin == 0 && cyMin == 0)
    {
        HDC        hdc;
        int        i;
        TEXTMETRIC tm;
        SIZE       size;


        hdc = GetDC(hwnd);

        // find minimum allowable window size by finding widest cell label string
        for (i = 0; i < 16; i ++)
        {
				GetTextExtentPoint(hdc, aszLabel[i], lstrlen(aszLabel[i]), &size);
            cxMin = cyMin = max(cxMin, size.cx);
        }

        // add one-character-wide margin
        GetTextMetrics(hdc, &tm);
        cxMin += 2 * tm.tmAveCharWidth;
        cyMin += 2 * tm.tmAveCharWidth;

        ReleaseDC(hwnd, hdc);
    }
    lpmmi->ptMinTrackSize.x = 4 * cxMin;
    lpmmi->ptMinTrackSize.y = 4 * cyMin;
*/
    return 0;
}

//
//  FUNCTION: MsgDestroy(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Kills the timer and posts the quit message.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_DESTROY (Unused)
//    wparam    - Extra data (Unused)
//    lparam    - Extra data (Unused)
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#pragma argsused
LRESULT MsgDestroy(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
	if(trace)
		fprintf(stderr,"DESTROY MSG\n");
//    KillTimer(hwnd, idTimer);       // Stops the timer
    PostQuitMessage(0);

	 return 0;
}


//
//  FUNCTION: MsgLButtonDown(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display left mouse button down message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_LBUTTONDOWN (Unused)
//    wparam    - Key flags
//    lparam    -
//      LOWORD - x position of cursor
//      HIWORD - y position of cursor
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#pragma argsused
LRESULT MsgLButtonDown(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
WORD xPos = LOWORD(lparam);  // horizontal position of cursor
WORD yPos = HIWORD(lparam);  // vertical position of cursor

if(is_mouse)
	{
   if(trace)
   	fprintf(stderr,
		  "WM_LBUTTONDOWN: %x, %d, %d\n",
		  wparam, LOWORD(lparam), HIWORD(lparam)
	 		);
	   InputXpos=xPos;
		InputYpos=yPos;
		InputClick=1;
		InputFlag=1;
	   InputChar='\b';
    }
    return 0;
}

//
//  FUNCTION: MsgLButtonDoubleClick(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display left mouse button double click message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_LBUTTONDOBLECLICK (Unused)
//    wparam    - Key flags
//    lparam    -
//      LOWORD - x position of cursor
//      HIWORD - y position of cursor
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#pragma argsused
LRESULT MsgLButtonDoubleClick
	 (HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
if(trace)
	 fprintf(stderr,
		  "WM_LBUTTONDBLCLK: %x, %d, %d\n",
		  wparam, LOWORD(lparam), HIWORD(lparam)
    );
    return 0;
}


//
//  FUNCTION: MsgRButtonDown(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display right mouse button down message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_RBUTTONDOWN (Unused)
//    wparam    - Key flags
//    lparam    -
//      LOWORD - x position of cursor
//      HIWORD - y position of cursor
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#pragma argsused
LRESULT MsgRButtonDown(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
WORD xPos = LOWORD(lparam);  // horizontal position of cursor
WORD yPos = HIWORD(lparam);  // vertical position of cursor
if(is_mouse)
	{
   if(trace)
	 fprintf(stderr,
		  "WM_RBUTTONDOWN: %x, %d, %d\n",
        wparam, LOWORD(lparam), HIWORD(lparam)
    	);
	InputXpos=xPos;
   InputYpos=yPos;
   InputClick=2;
   InputFlag=1;
   InputChar='\b';
   }
    return 0;
}


//
//  FUNCTION: MsgRButtonDoubleClick(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display right mouse button double click message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_RBUTTONDOUBLECLICK (Unused)
//    wparam    - Key flags
//    lparam    -
//      LOWORD - x position of cursor
//      HIWORD - y position of cursor
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#pragma argsused
LRESULT MsgRButtonDoubleClick
	 (HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
if(trace)
	 fprintf(stderr,
        "WM_RBUTTONDBLCLK: %x, %d, %d\n",
        wparam, LOWORD(lparam), HIWORD(lparam)
    );
    return 0;
}


//
//  FUNCTION: MsgKeyDown(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display key down message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_KEYDOWN (Unused)
//    wparam    - Virtual Key Code
//    lparam    - Key Data
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#pragma argsused
LRESULT MsgKeyDown(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
if(trace)
    fprintf(stderr,
        "WM_KEYDOWN: %x, %x, %x\n",
        wparam, LOWORD(lparam), HIWORD(lparam)
    );

    return 0;
}


//
//  FUNCTION: MsgKeyUp(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display key up message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_KEYUP (Unused)
//    wparam    - Virtual Key Code
//    lparam    - Key Data
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#pragma argsused
LRESULT MsgKeyUp(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
if(trace)
    fprintf(stderr,
        "WM_KEYUP: %x, %x, %x\n",
        wparam, LOWORD(lparam), HIWORD(lparam)
    );
//	 InputChar='\0';
    return 0;
}


//
//  FUNCTION: MsgChar(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display character recieved message and its parameters.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_CHAR (Unused)
//    wparam    - Character Code
//    lparam    - Key Data
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#pragma argsused
LRESULT MsgChar(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
if(trace)
    fprintf(stderr,
        "WM_CHAR: %c, %x, %x\n",
        wparam, LOWORD(lparam), HIWORD(lparam)
    		);
    InputChar=wparam;
    return 0;
}


//
//  FUNCTION: MsgTimer(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Display timer message and a current time.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_TIMER (Unused)
//    wparam    - The timer identifier
//    lparam    - NULL           (Unused)
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//

#pragma argsused
LRESULT MsgTimer(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
/*
    if ( wparam == TIMERID ) {
        wsprintf(
            TimerText,
            "WM_TIMER: %d seconds",
            nTimerCount += 5
        );
    }
*/
    return 0;
}

//
//  FUNCTION: MsgPaint(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Draw the strings for current messages.
//
//  PARAMETERS:
//    hwnd      - Window handle
//    uMessage  - WM_PAINT (Unused)
//    wparam    - Extra data (Unused)
//    lparam    - Extra data (Unused)
//
//  RETURN VALUE:
//    Always returns 0 - Message handled
//
//  COMMENTS:
//
//
static struct {LONG left,top,right,bottom;} forrepaint;
static int repaint_flag=0;

#pragma argsused
LRESULT MsgPaint(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
    PAINTSTRUCT ps;
    RECT rect;
	HDC  TempHdc;
	LONG left,top,right,bottom;

	if(trace)
		fprintf(stderr,"REPAINT MSG ");

    TempHdc = BeginPaint(hwnd, &ps);
    
	left=ps.rcPaint.left;
	top=ps.rcPaint.top;
	right=ps.rcPaint.right; 
	bottom=ps.rcPaint.bottom;

	if(animate && VirtualScreen && MbHdc )//Animujemy i jest juz bitmapa
		{	
		BitBlt(	TempHdc,
				left,top,
				right-left,//W_width*mulx,
				bottom-top,//W_height*muly,
				MbHdc,
				left,top,
				SRCCOPY);
		
		if(trace)
			fprintf(stderr,"-> BITBLT ");
		}
		else 
		{
		if(trace)
			fprintf(stderr,"-> FORCE REPAINT ");	
		
		if(repaint_flag==1)//Jeszcze aplikacja nie odczytala
			{
			if(forrepaint.left>left) forrepaint.left=left;
			if(forrepaint.top>top) forrepaint.top=top;
			if(forrepaint.right<right) forrepaint.right=right;
			if(forrepaint.bottom<bottom) forrepaint.bottom=bottom;
			}
			else
			{
			forrepaint.left=left;
			forrepaint.top=top;
			forrepaint.right=right;
			forrepaint.bottom=bottom;			
			repaint_flag=1;
			}
		
		InputChar='\r';//Tylko udaje ze cos wypelnia. 
				//Naprawde zleca aplikacji odbudowanie 

		if(trace)
			{
			fprintf(stderr," %ld %ld %ld %ld ",
				forrepaint.left,
				forrepaint.top,
				forrepaint.right,
				forrepaint.bottom);
			}
		}

    EndPaint(hwnd, &ps);
	if(trace)
		fprintf(stderr,"\n");
    return 0;
}

int repaint_area(int* x,int* y,int* width,int* height)
{
if(repaint_flag=1)
	{
	*x=forrepaint.left;
	*y=forrepaint.top;
	*width=forrepaint.right-forrepaint.left;
	*height=forrepaint.bottom-forrepaint.top;
	repaint_flag=0;
	return 0;
	}
	else
	return -1;//Nie bylo do nic odczytania
}

#pragma argsused
LRESULT MsgSize(HWND hwnd, UINT uMessage, WPARAM wparam, LPARAM lparam)
{
  WORD	fwSizeType = wparam;      // resizing flag
  WORD	nWidth = LOWORD(lparam);  // width of client area
  WORD	nHeight = HIWORD(lparam); // height of client area
  WORD	n_mulx=(nWidth-ini_col*_char_width())/ini_width;
  WORD	n_muly=(nHeight-ini_row*_char_height())/ini_height;
  if(trace)
	fprintf(stderr,"RESIZE to %u %u",nWidth,nHeight);
  if(n_mulx<1) n_mulx=1;
  if(n_muly<1) n_muly=1;
  if(n_mulx!=mulx || n_muly!=muly)// Rozmiar faktycznie sie zmienil
  	  {
      W_width=ini_width+(ini_col*_char_width())/n_mulx;
      W_height=ini_height+(ini_row*_char_height())/n_muly;
      mulx=n_mulx;muly=n_muly;
	  if(trace)
		fprintf(stderr," MUL X=%u Y=%u ",mulx,muly);
      FreeResources();//DC , Pens , Brushes
	  assert(MbHdc==0);
	  assert(VirtualScreen==NULL);
	  if(animate)
			{
			if(trace)
				fprintf(stderr," FORCE REPAINT");
			InputChar='\r';
			}
      }

  if(trace)
	fprintf(stderr,"\n");

  return 0;
}



//
//  FUNCTION: InitInput(HWND)
//
//  PURPOSE: Set up the rectangles for dispay of each type of message
//
//  PARAMETERS:
//    hwnd   - Window handle
//
//  RETURN VALUE:
//    Always returns TRUE - Sucess
//
//  COMMENTS:
//
//

BOOL InitInput(HWND hwnd)
{
    HDC hdc;
    TEXTMETRIC tm;
    RECT rect;
    int  dyLine;

   return TRUE;
}
