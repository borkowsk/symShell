/* Najprostrzy interface wizualizacyjny */
/* Wyswietla pod X-windows za pomoca biblioteki X11 */
/* symshx11.c */
#include "platform.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <assert.h>
#include <math.h>
#include <X11/Xlib.h> 
#include <X11/Xutil.h>
#include <X11/Xos.h> 
#include <X11/Xatom.h> 

#include "symshell.h"
#include "icon.h" 

#define BITMAPDEPTH 1 
#define TOO_SMALL 0 
#define BIG_ENOUGH 1 

#define FALSE 0
#define NODATA '\0'

/* For close_plot() */
static int opened=0; /* Dla close plot. Zerowane tez gdy "broken-pipe" */
extern int WB_error_enter_before_clean;

 
/* These are used as arguments to nearly every Xlib routine, so it 
  * saves routine arguments to declare them global; if there were 
  * additional source files, they would be declared extern there */ 
 static Display *display=0; 
 static int screen_num; 

 /* progname is the string by which this program was invoked; this 
  * is global because it is needed in most application functions */ 
 static char *progname="WB SYMULATION NAME NOT SET"; 
 static char *window_name = "WB X-window symulation shell";  
 static char *icon_name = "WB-sym-shell";  
 static size_t largc=0;		/* Do zapamietania przekazanych przez funkcje */
 static char**  largv=NULL;
 static int 	buffered=0;       /* Czy okno jest buforowane pixmapa */
 static int     animate=0;	  /* Czy odswierznaie tylko gdy flush czy na biezaco */
 static unsigned int mulx=1,muly=1;       /* Multiplication of x & y */
 static unsigned int org_width,org_height;/* Starting Window size */  
 static int ini_a,ini_b,ini_ca,ini_cb;	 /* Konieczne do dzialania 
						screen_width() i screen_heiht() i zmiany rozmiaru*/

 static Window win;  
 static unsigned int width,height;      /* Window size */  
 static int x, y;                       /* Window position */ 
 static unsigned int default_deph = 8;  /* Window deph. 8=>256 colors */ 
 static unsigned int border_width = 4;  /* Four pixels */  
 static unsigned int display_width, display_height;  
 static unsigned int icon_width, icon_height;  
 static Pixmap icon_pixmap; 
 static unsigned short alloc_cont=0; /* Sygnalizuje ze pixmapa zaostala zaalokowana */ 
 static Pixmap cont_pixmap=0;
 static XSizeHints *size_hints;  
 static XIconSize *size_list;  
 static XWMHints *wm_hints;  
 static XClassHint *class_hints;  
 static XTextProperty windowName, iconName; 

 static int count;  
 static KeySym thekey;		/* ??? */
 static GC gc=0;  		/* Kontekst graficzny */
 static int ResizeFont=0;	/* Czy probuje zmieniac rozmiar fontu */		
 static XFontStruct *font_info=NULL;   /* Aktualny font */
 static unsigned ori_font_width = 8;
 static unsigned ori_font_height = 16;
 static unsigned font_width = 0;
 static unsigned font_height = 0;
 static char *display_name = NULL; /* Nazwa wyswietlacza - do odczytania */ 
 static int window_size = TOO_SMALL; /* BIG_ENOUGH or TOO_SMALL to display contents */

 static struct XRect{
	int x,y,width,height;
	} last_repaint_data;
 static int repaint_flag=0;

 static unsigned long Black,White; 
 static Colormap colormap=0;
 static int CurrForeground=-1;      /* Index to last set color */
 static unsigned  bacground=0;      /* Index koloru tla */
 static unsigned long Scale[257]; 
 static void SetScale(XColor RGBarray[]);

 static int  buforek[2]; /* Bufor na znaki z klawiatury - tylko 0 jest przekazywany */
 static char bfirst=0;  /* Zmienne na implementacje cykliczna */
 static char blast=0;   /* nie uzywane... */

 static char trace=0;
 static int mouse=0;

 static struct { int  flags, x, y;unsigned buttons;} 
										LastMouse={0, 0, 0, 0};

 static int error_limit=3; /* Limit odeslanych bledow od x-serwera */
 static int error_count=3; /*antylicznik bledow - gdy 0 - wyjscie */
 static int DelayAction=0; /* Sterowanie zasypianiem jesli program czeka */
 static int UseGrayScale=0;
 
static int pipe_break=0;
void SigPipe(int num)
{
pipe_break=1;
opened=0;
signal(num,SIG_IGN);
fprintf(stderr,"SYMSHELL GOT A SIGNALL #%d\n",num); 
}

int mouse_activity(int yes)
/* Ustala czy mysz ma byc obslugiwana. 
W X11 zawsze jest, ale mozna ja ignorowac */
{
int pom=mouse;
mouse=yes;
return pom;
}

unsigned get_background()
{
return bacground;
}

void set_background(unsigned char c)
/* Ustala index koloru do czyszczenia itp */
{
/*if(c>=0 && c<=256)*/
bacground=c;
}

void buffering_setup(int _n)
{
if(_n)
	animate=1;
	else
	animate=0;
if(animate)	/* Musi byc wlaczona bitmapa buforujaca */
	buffered=1;/* zeby mozna bylo na nia pisac */
}

unsigned get_buffering()
{
return animate;
}

static void CloseAll()
{
 if(display==0)
	{
	if(trace) fprintf(stderr,"Traing to free resources of NULL display!\n");
	return;
	}

 if(font_info){
 	XUnloadFont(display, font_info->fid);  
	font_info=NULL; }

 if(gc!=0){
 	XFreeGC(display, gc);  
	gc=0; }

#ifndef __MSDOS__
if(colormap!=0){
 	XUninstallColormap(display,colormap);
 	XFreeColormap(display,colormap);
	colormap=0;}
#endif

if(alloc_cont)
 	{
	XFreePixmap(display,cont_pixmap);
	if(trace)
  		fprintf(stderr,"%s %x\n","FREE PIXMAP",cont_pixmap); 
	}

XCloseDisplay(display);  
display=0;
}

void close_plot()
/* --------//---------- zamkniecie grafiki/semigrafiki */
{
if(opened)
    {
    if(WB_error_enter_before_clean)
		{
		char* kom="(Press ANY KEY to close graphix)";
		/* width,height of Window at this moment */
		print(screen_width()/2-(strlen(kom)*char_width('X'))/2,screen_height()/2,kom);
		flush_plot();
		fprintf(stderr,"(See at window %s )\n",window_name);
		get_char();
		WB_error_enter_before_clean=0;
		}
    CLOSE:		
    CloseAll();
    opened=0;
    }
}
 
static void ResizeBuffer(unsigned int nwidth,unsigned int nheight)
/* Alokuje pixmape na zawatosc okna */
{
 if(alloc_cont && cont_pixmap!=0)
 	{
	XFreePixmap(display,cont_pixmap);
	alloc_cont=cont_pixmap=0;
 	}
 	
 if(trace)
   {
   printf("%s %dx%d\n","ALLOC PIXMAP",nwidth,nheight); 
   /*getchar();*/
   }
   
 cont_pixmap=XCreatePixmap(display,win,nwidth,nheight,default_deph);
 assert(cont_pixmap);
 alloc_cont=1;
 XSetForeground(display, gc, Black); 
 CurrForeground=-1; 
 XFillRectangle(display,cont_pixmap , gc, 0, 0, nwidth+1, nheight+1);
 if(trace)
	{
	XFlush(display);
	printf("PIXMAP %x DISPLAY %x WIN %x %s\n",
			cont_pixmap,
			display,
			win,
			"OK?");			
	/*getchar();*/
	} 	
}

static void load_font(font_info,gc)  
 XFontStruct **font_info;  
 GC *gc;
 {  
    char fontname[256]; 
    XFontStruct* l_font_info;
  
    sprintf(fontname,"%dx%d",ori_font_width,ori_font_height);

  if(!ResizeFont && (*font_info) != NULL ) return;

  /* Load font and get font information structure */  
  if ((l_font_info = XLoadQueryFont(display,fontname)) == NULL)  
    {  
       (void) fprintf( stderr, "%s: Cannot open %s font\n",  
             progname,fontname);  
       if( (*font_info)== NULL )
       		exit( -1 ); /* Nie ma zadnego fontu */
		else
		return ;    /* Pozostaje stary */
    } 

  if( (*font_info) != NULL ) /* Usuwa stary font */
  	XUnloadFont(display, (*font_info)->fid);
  (*font_info)=l_font_info;   /* zapamietuje nowy do uzycia */
 
   /* Specify font in gc */  
   XSetFont(display, *gc, l_font_info->fid );

  /* Get string widths & hight */  
    font_width = XTextWidth(*font_info, "X", 1);  
    font_height = (*font_info)->ascent + (*font_info)->descent;
 
 if(trace)
  printf("%s:font %ux%u\n",icon_name,font_width,font_height); 
 }  

static void TooSmall(win, gc, font_info)  
Window win;  
GC gc;  
XFontStruct *font_info;  
{  
 char *string1 = "Too Small";
 int y_offset, x_offset;    
 y_offset = font_info->ascent + 2;  
 x_offset = 2;  
 /* Output text, centered on each line */  
 XDrawString(display, win, gc, x_offset, y_offset, string1,  
          strlen(string1));  
}  

static int ErrorHandler(Display *xDisplay, XErrorEvent *event)
{
    char buf[80];

    fprintf(stderr,"\nReceived X error!\n");
    fprintf(stderr,"Ecode  : %d", event->error_code);
    fprintf(stderr,"\tRequest: %d", event->request_code);
    fprintf(stderr,"\tMinor: %d", event->minor_code);
    XGetErrorText(xDisplay, event->error_code, buf, 80);
    fprintf(stderr,"\tEtext : '%s'\n", buf);
    
    error_count--;
    if(!error_count)
    		exit(event->error_code);	
    return 0;
}

 
static void place_graphics(win, gc,area_x,area_y,area_width, area_height)  
Window win;  
GC gc;  
int area_x,area_y;
unsigned int area_width, area_height;
{  
  XCopyArea(display, cont_pixmap, win, gc,
           area_x/*src_x*/, area_y /*src_y*/, 
           area_width, area_height,  
           area_x/*dest_x*/, area_y/*dest_y*/);
}
  
static int Read_XInput()
{
static int first=1;
static int buffer_empty=1;

XEvent report;  /* Miejsce na odczytywane zdazenia */

if(pipe_break)	/* Musi zwrocic EOF */	
	return 1;


 /* Get events, use first to display text and graphics */  
      XNextEvent(display, &report); 
      if(report.xany.send_event==FALSE)
       {
       switch  (report.type) {  

       case Expose: 
       if(trace)
         printf("EXPOSE: %s #%d x=%d y=%d %dx%d\n",
			icon_name,
			report.xexpose.count,
			report.xexpose.x,
			report.xexpose.y,
			report.xexpose.width,
			report.xexpose.height); 

       if(first==1)
	    {
	    /* Pierwszy event typu Exposure jest powielany, */   
	    /* Zeby zawsze cos bylo do odebrania */ 
	    first=0;
            XSendEvent(display,win,FALSE,  ExposureMask ,&report);  	   
	    }

          /* Unless this is the last contiguous expose,  
           * don't draw the window */  
	  /* if (!buffered && report.xexpose.count != 0)  
            			 break;  */
         
          /* If window too small to use */  
       if(window_size == TOO_SMALL)  
             TooSmall(win, gc, font_info);  
          else 
	    {  
            XSetForeground(display, gc, Scale[bacground]);
			CurrForeground=-1; 
    
			XFillRectangle(display,win , gc, 			
			report.xexpose.x,
			report.xexpose.y,
			report.xexpose.width,
			report.xexpose.height);

	   if( repaint_flag==1 || (!buffered) || buffer_empty )
		{
		/* SUmuje z marginesem, takz zeby pokrywalo wszytkie expos'y */
		if(last_repaint_data.x>report.xexpose.x)
			last_repaint_data.x=report.xexpose.x;
		
		if(last_repaint_data.y>report.xexpose.y)
			last_repaint_data.y=report.xexpose.y;
		
		/* Tu jest chyba zle */
		if(last_repaint_data.width<report.xexpose.width)
			last_repaint_data.width+=report.xexpose.width;
		
		if(last_repaint_data.height<report.xexpose.height)
			last_repaint_data.height+=report.xexpose.height;

		repaint_flag=1;/* Sa juz dane dla repaint */
        
		/* Set information for main program about refresh screen */
		if(report.xexpose.count == 0 ||  buffer_empty )
			{
			if(trace)
		    		printf("EXPOSE force repaint\n");
			buforek[0]='\r';
			buffer_empty=0;
			
			}
		}
		else
		{
           	/* Refresh from pixmap buffer */ 
		if(trace)
		    printf("EXPOSE DOING BITBLT\n");
                place_graphics(win, gc ,
			report.xexpose.x,report.xexpose.y,
			report.xexpose.width,report.xexpose.height
			 ); 
		DelayAction=0;/* Pojawila sie aktywnosc. Nie nalezy spac! */
		}
       }  
       break; 

       case MappingNotify:
 	   XRefreshKeyboardMapping((XMappingEvent *)&report );
 	   DelayAction=0;/* Pojawila sie aktywnosc. Nie nalezy spac! */
           break;

       case ConfigureNotify:  
       	   DelayAction=0;/* Pojawila sie aktywnosc. Nie nalezy spac! */
       	   if(trace)
          	printf("CONFIGURE: %s=%dx%d scale: x=%d:1 y=%d:1 ",
			icon_name,
			width,height,mulx,muly);
	   
          /* Window has been resized; change width  
           * and height for next Expose */

	  if( width== report.xconfigure.width &&
              height== report.xconfigure.height)
		{
        	if(trace)
			fprintf(stderr,"The same.\n");
		break; /* Nic sie nie zmienilo */
		}

          width = report.xconfigure.width;  
          height = report.xconfigure.height;  

      if ((width < size_hints->min_width) ||  
                (height < size_hints->min_height))
	     {  
             window_size = TOO_SMALL;  
             if(trace)
	      	fprintf(stderr,"To small!\n");
	     }
          else  
	     {
             window_size = BIG_ENOUGH; 
	     /*mulx=width/org_width; STARA WERSJA
	     muly=height/org_height;*/
	     
	     mulx=(width-ini_ca*font_width)/ini_a;
	     muly=(height-ini_cb*font_height)/ini_b;
	     
	     load_font(&font_info,&gc); /* New font - size changed */
	     
	     if(buffered)
		 {
	        ResizeBuffer(width,height);
			buffer_empty=1;
		 }
		
	     if(trace)
	     	printf("->%dx%d scale: x=%d:1 y=%d:1 \n",width,height,mulx,muly); 
	     } 
          break;  

       case ButtonPress:
       	    DelayAction=0;/* Pojawila sie aktywnosc. Nie nalezy spac! */ 
            if(mouse)
		{ 		
		buforek[0]='\b';
		LastMouse.flags=1;
		LastMouse.x=report.xbutton.x;
		LastMouse.y=report.xbutton.y;
		LastMouse.buttons=report.xbutton.button;
		if(trace)
		    printf("ButtonPress:x=%d y=%d b=X0%x\n",
		    LastMouse.x,LastMouse.y,LastMouse.buttons  );
		}
	    break;

       case KeyPress:  
	    {
	    char Bufor[8];
	    unsigned KeyCount=XLookupString((XKeyEvent *)&report,Bufor,sizeof(buforek),&thekey,0);
	    DelayAction=0;/* Pojawila sie aktywnosc. Nie nalezy spac! */ 
	    
	    *buforek=*Bufor;/* Na zewnatrz widziane w buforku */
	    
	    if(KeyCount!=1)
		 *buforek=NODATA;
	    
	    if(trace)
		 fprintf(stderr,"KeyPress:%c %x \n ",*Bufor,(int)(*Bufor));
        
		if(*Bufor==0x3 || *Bufor==0x4)/* User przerwal w oknie X11 */
			*buforek=EOF;
	    } break;
       
	   default:  
          /* All events selected by StructureNotifyMask  
           * except ConfigureNotify are thrown away here,  
           * since nothing is done with them */  
          break;  
       } /* End switch */ 
       } 
       else
       XSendEvent(display,win,FALSE,  ExposureMask | KeyPressMask |
 	  ButtonPressMask | StructureNotifyMask,&report);    /* Zeby zawsze cos bylo do odebrania */ 

return 0;
}
 
 static void getGC(win, gc, font_info)  
 Window win;  
 GC *gc;  
 XFontStruct *font_info;  
 { 
    XColor pom,Array[256]; 
    unsigned long valuemask = 0,i; /* Ignore XGCvalues and  
                          * use defaults */  
    XGCValues values;  
    unsigned int line_width = 6;  
    int line_style = LineOnOffDash;  
    int cap_style = CapRound;  
    int join_style = JoinRound;  
    int dash_offset = 0;  
    static char dash_list[] = {12, 24};  
    int list_length = 2;  
    
    /* Create default Graphics Context */  
    *gc = XCreateGC(display, win, valuemask, &values);  
   
    /* Specify black foreground since default window background  
     * is white and default foreground is undefined */
    Black=BlackPixel(display,screen_num); 
    White=WhitePixel(display,screen_num);  
    XSetForeground(display, *gc, Black);  
    
    /* Set line attributes */  
    XSetLineAttributes(display, *gc, line_width, line_style,  
          cap_style, join_style);  
    /* Set dashes */  

    /*
	XSetDashes(display, *gc, dash_offset, dash_list, list_length);  
     */
     
    /* Set Color scale */
    Scale[0]=BlackPixel(display,screen_num); 
    Scale[255]=WhitePixel(display,screen_num);  
    
#ifdef __MSDOS__
    colormap=XDefaultColormap(display,screen_num); /* W emulacji nie ma map */
#else
    colormap=XCreateColormap(display,win,DefaultVisual(display,screen_num),AllocNone);
#endif

    XSetWindowColormap(display,win,colormap);

    pom.red=pom.green=pom.blue=0;
    XAllocColor(display,colormap,&pom);
    Black=pom.pixel;
    Scale[0]=pom.pixel;
    XSetForeground(display, *gc, Black); 
   /*fprintf(stderr,"BLACK %u\n",Black);*/
	
    pom.red=pom.green=pom.blue=0xffff;
    XAllocColor(display,colormap,&pom);
    White=pom.pixel;
    Scale[255]=pom.pixel;
    XSetBackground(display, *gc, White); 
    /*fprintf(stderr,"WHITE %u\n",White);*/

    SetScale(Array);	
    for(i=1;i<255;i++)
	{
	pom=Array[i];
	
	pom.red*=256;
        pom.blue*=256;
        pom.green*=256;
	
	XAllocColor(display,colormap,&pom);
	
	Scale[i]=pom.pixel;	
	}
     XInstallColormap(display,colormap);
 }  


void shell_setup(char* title,int iargc,char* iargv[])
{
/* Przekazanie parametrow wywolania */
int i;
largc=iargc;
largv=iargv;
progname=largv[0]; 
window_name = title;  
icon_name = title;  
for(i=1;i<largc;i++)
	{
	if(strncmp(largv[i],"-h",2)==0)
		{
		printf("SYMSHELL for X11 enviroment. Compiled %s\n",__DATE__);
		printf("Supported swithes:\n\t -mapped+/-] \n\t -buffered[+/-]"
				"\n\t -trace[+/-]"
				"\n\t -bestfont[+/-] "
				"\n\t -traceevt[+/-] "
				"\n\t -gray[+/-] "
				"\n"); 
		printf("You can always gracefully break the program sending\n"
		" the  ^C  or  ^D  key	 to  g_r_a_p_h_i_c_s  w_i_n_d_o_w !\n"
		"Other methods break the program immediatelly by exit call!\n");		
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
		/* Jesli user chec tryb mapowania to nie bedziemy */
		animate=0; /* animowac */
		}
	else
	if(strncmp(largv[i],"-buffered",9)==0)
		{
		animate=(largv[i][9]=='+')?1:0;
	        printf("Buffered is %s\n",(animate?"ON":"OFF"));
		/* Musi byc wlaczona bitmapa buforujaca */
		buffered=animate;/* zeby mozna bylo na nia pisac */
		}
	else
	if(strncmp(largv[i],"-bestfont",9)==0)
		{
		ResizeFont=(largv[i][9]=='+')?1:0;
	        printf("Search best font is %s\n",(ResizeFont?"ON":"OFF"));
		}
	else
	if(strncmp(largv[i],"-traceevt",9)==0)
		{
		trace=(largv[i][9]=='+')?1:0;
	        printf("Trace events is %s\n",(trace?"ON":"OFF"));
		}
	}
}


int init_plot(int a,int b,int ca,int cb)
/* typowa dla platformy inicjacja grafiki/semigrafik
 a,b to wymagane rozmiary ekranu */
{
int* disp_depht;
int  disp_depht_num=0,i;
ini_a=a;
ini_b=b;
ini_ca=ca;
ini_cb=cb;

    if (!(size_hints = XAllocSizeHints())) {  
       fprintf(stderr, "%s: failure allocating memory", progname);  
         exit(-2);  return 0;
    }  
    if (!(wm_hints = XAllocWMHints())) {  
       fprintf(stderr, "%s: failure allocating memory", progname);  
         exit(-2);  return 0;
    }  
    if (!(class_hints = XAllocClassHint())) {  
       fprintf(stderr, "%s: failure allocating memory", progname);  
         exit(-2); return 0; 
    } 
 
    /* Connect to X server */  
    if ( (display=XOpenDisplay(display_name)) == NULL )  
    {  
       fprintf( stderr, "'%s': cannot connect to X server '%s'\n",  
             progname, XDisplayName(display_name));  
       exit( -1 );  return 0;
    } 
 
    /* Get screen size from display structure macro */  
    screen_num = DefaultScreen(display);  
    display_width = DisplayWidth(display, screen_num);  
    display_height = DisplayHeight(display, screen_num);  
	disp_depht = XListDepths(display, screen_num, &disp_depht_num);
    if(trace)
    	{
    	printf("Available display depths:");
    	for(i=0;i<disp_depht_num;i++)
    		printf("%d ",disp_depht[i]);
    	putchar('\n');	
    	}	
    			
    /* Search for depht */
    for(i=0;i<disp_depht_num;i++)
    	if(disp_depht[i]>=default_deph)
    		{
    		default_deph=disp_depht[i];/*Pierwszy >= wymaganemu */
    		break;
    		}
   if(trace)
   	printf("Select depth %d\n",default_deph);
   	
    /* Note that in a real application, x and y would default  
     * to 0 but would be settable from the command line or  
     * resource database */ 
   org_width = width = a+ca*ori_font_width; 
   org_height = height = b+cb*ori_font_height;  
   x=y=0;
   printf("%s=%dx%d\n",icon_name,width,height);

   size_hints->min_width = width;   /* Startup width/haight */
   size_hints->min_height = height; /* are minimal */ 

 /* Create opaque window */  
 #ifdef CREATE_FULL 
   win = XCreateWindow(display, 
   		RootWindow(display,screen_num),  
          	x, y, width, height, border_width,
	  	default_deph /* Window deph */,
          	InputOutput /*WindowClass*/,
	  	CopyFromParent /* Visual */,
	  	0	/* Valuemask */,
	  	0     /* Atributes */
	 	);
 #else
    win = XCreateSimpleWindow(display, 
    			RootWindow(display,screen_num),  
          		x, y, width, height, border_width, 
          		BlackPixel(display, screen_num), 
          		WhitePixel(display,screen_num)
          		); 
 #endif
  
    window_size = BIG_ENOUGH ;
    /* Get available icon sizes from window manager */  
    if (XGetIconSizes(display, RootWindow(display,screen_num),&size_list, &count) == 0){  
       (void) fprintf( stderr, "%s: Window manager didn't set "  
                                "icon sizes - using default.\n", progname); 
     }
    else {  
       ;  
       /* A real application would search through size_list  
        * here to find an acceptable icon size and then  
        * create a pixmap of that size; this requires that  
        * the application have data for several sizes of icons */  
    } 
  
/* Create pixmap of depth 1 (bitmap) for icon */  
    icon_pixmap = XCreateBitmapFromData(display, win,  
			WB_icon_bitmap_bits,WB_icon_bitmap_width,WB_icon_bitmap_height); 
 
    /* Set size hints for window manager; the window manager  
     * may override these settings   
     * Note that in a real application, if size or position  
     * were set by the user, the flags would be USPosition  
     * and USSize and these would override the window manager's  
     * preferences for this window   
     * x, y, width, and height hints are now taken from  
     * the actual settings of the window when mapped; note  
     * that PPosition and PSize must be specified anyway */  
    size_hints->flags = PPosition | PSize | PMinSize;  
   

    /* These calls store window_name and icon_name into  
     * XTextProperty structures and set their other fields  
     * properly */  
    if (XStringListToTextProperty(&window_name, 1, &windowName) == 0) {  
       (void) fprintf( stderr, "%s: structure allocation for "  
                               "windowName failed.\n", progname);  
       exit(-1);return 0;  
    }  
    if (XStringListToTextProperty(&icon_name, 1, &iconName) == 0) {  
       (void) fprintf( stderr, "%s: structure allocation for "  
                               "iconName failed.\n", progname);  
       exit(-1);  return 0;  
    }  
    wm_hints->initial_state = NormalState;  
    wm_hints->input = True;  
    wm_hints->icon_pixmap = icon_pixmap;  
    wm_hints->flags = StateHint | IconPixmapHint | InputHint;  
    class_hints->res_name = progname;  
    class_hints->res_class = "Basicwin";  
    XSetWMProperties(display, win, &windowName, &iconName,  
          largv, largc, size_hints, wm_hints,  
          class_hints);  
   /* } ??? */ 

    /* Select event types wanted */  
    XSelectInput(display, win,
	  ExposureMask | KeyPressMask |
 	  ButtonPressMask | StructureNotifyMask );  

    /* ...GC for text and drawing */  
    getGC(win, &gc, font_info);  
    load_font(&font_info,&gc);

    /* Display window */ 
    buforek[0]=NODATA; 
    XSetErrorHandler(ErrorHandler);
    XMapWindow(display, win); 
    
    /* Alloc pixmap for contens buffering */
    if(buffered)
    	ResizeBuffer(width,height);

    opened=1;
    atexit(close_plot);
    signal(SIGPIPE,SigPipe); 
    
    while(!input_ready()); /* Wait for expose */ 

    /* Czysci zeby wprowadzic ustalone tlo */
    if(trace)
	printf("Background is %d\n",(int)bacground);
    clear_screen();

    return 1;
}

int screen_height()
{
return ini_b+(ini_cb*font_height)/muly;/* Window size */  
}

int  screen_width()
{
return ini_a+(ini_ca*font_width)/mulx;/* Window size */  
}

int  char_height(char znak)
{
int pom=font_height/muly;
if(pom<1)pom=1;
return pom;
}

int  char_width(char znak)
{
int width;
char pom[2];
pom[0]=znak;pom[1]='\0';
width=XTextWidth(font_info,pom, 1)/mulx;
if(width<1)width=1;
return width;
}

int  string_height(const char* str)
/* BARDZO PRYMITYWNIE! */
/* Aktualne rozmiary lancucha */
{
return char_height(*str);
}

int  string_width(const char* str)
/* ...potrzebne do jego pozycjonowania */
{
int pom=XTextWidth(font_info,str,strlen(str))/mulx;
if(pom<1)pom=1;
return pom;
}

void flush_plot()
/* --------//---------- uzgodnienie zawartosci ekranu */
{
/* Read_XInput(); * Oraz sprawdzenie czy nie ma zdarzen */
if(trace)
	printf("FLUSH %s",icon_name);
if(animate)
	{/* Wyswietlenie bitmapy */
	XCopyArea(display, cont_pixmap, win, gc,
           0/*src_x*/, 0/*src_y*/, 
           width, height,  
           0/*dest_x*/, 0/*dest_y*/);
        if(trace)
		printf("DOING BITBLT"); 
	}
XFlush(display);
if(trace)
	printf(" XFLUSH\n");
error_count=error_limit;	
}

/* GETTING INPUT */
/* Za pierwszym razem zwraca '\r'
zeby zasygnalizowac ze trzeba wyrysowac ekran */
static int first_to_read=0;



int  input_ready()
/* zalezna od platformy funkcja sprawdzajaca czy jest wejscie */
{
if(first_to_read) 
	return 1;

Read_XInput(); /* Sprawdzenie czy nie ma zdarzen */

if(buforek[0]!=NODATA)
	return 1;
   else
	return 0;
}

int  set_char(int c)	
/* Odeslanie znaku na wejscie - zwraca 0 jesli nie ma miejsca */
{
if(first_to_read!=0)/* Nieodebrano */
		return 0;
first_to_read=c;
return 1;
}

int  get_char()
/* odczytywanie znakow sterowania */
{
int pom;

if(first_to_read!=0)
	{int pom=first_to_read ;
	 first_to_read=0;
	 return pom;}

DelayAction=0;

if(pipe_break==1)
	{
	if(trace)
		fprintf(stderr,"'pipe_break' flag detected in get_char() in '%s'.\n",icon_name);
	return EOF;
	}

/* Fist 10000 times doing fast. Nexts much slower. */
while(buforek[0]==NODATA)
	{
	Read_XInput();
	if(trace)
	if(DelayAction%1000==0)
		fprintf(stderr,"%s[%d] %d waiting!\n",icon_name,getpid(),DelayAction);
	sleep(DelayAction/10000);/* Po 10tysiacach obrotow petli wpada w stan czekania */
	if(++DelayAction>19999)
		{
		DelayAction=10000;
		}
	} 
pom=buforek[0];
buforek[0]=NODATA;
return pom;
}


/* Used for redraw & in print */

static char straznik1=0x77;
static char bufor[1024];
static char straznik2=0x77;
static int ox,oy;

void print(int x,int y,char* format,...)
/* ---//--- wyprowadzenie tekstu na ekran */
{
size_t len=0;
unsigned font_height=0;

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
/*
    width1 = XTextWidth(font_info, string1, len1);  
*/
    font_height = font_info->ascent + font_info->descent; 
 
    /* Output text, centered on each line */
 
    x*=mulx; /* Multiplicaton of coordinates */
    y*=muly; /* if window is bigger */
  
    ox=x;oy=y;
    CurrForeground=-1;
    XSetForeground(display,gc,Scale[0]);
    XSetBackground(display,gc,Scale[bacground]);
    
if(!animate)
    XDrawImageString(display, win, gc, x , y+font_height ,  bufor, len); 
if(buffered)
    XDrawImageString(display, cont_pixmap, gc, x , y+font_height ,  bufor, len);

}

void printc(int x,int y,
    unsigned char fore,unsigned char back,
    char* format,...)
{
size_t len=0;
unsigned font_height=0;

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
/*
    width1 = XTextWidth(font_info, string1, len1);  
*/
    font_height = font_info->ascent + font_info->descent; 
 
    /* Output text, centered on each line */
 
    x*=mulx; /* Multiplicaton of coordinates */
    y*=muly; /* if window is bigger */
  
    ox=x;oy=y;
    CurrForeground=-1;
    XSetForeground(display,gc,Scale[fore]);
    XSetBackground(display,gc,Scale[back]);
if(!animate)
    XDrawImageString(display, win, gc, x , y+font_height ,  bufor, len);
if(buffered)
    XDrawImageString(display, cont_pixmap, gc, x , y+font_height ,  bufor, len);
}

void plot(int x,int y,unsigned char c)
/* ----//--- wyswietlenie punktu na ekranie */
{
x*=mulx; /* Multiplicaton of coordinates */
y*=muly; /* if window is bigger */
if(c!=CurrForeground)
   { 
   CurrForeground=c;
   XSetForeground(display,gc,Scale[c]);
   }
if(mulx>1 || muly>1)
   {
   if(!animate)
   	XFillRectangle(display, win, gc, x, y, mulx, muly); 
   if(buffered)
   	XFillRectangle(display, cont_pixmap, gc, x, y, mulx, muly); 
   }
   else 
   {
   if(!animate)
   	XDrawPoint(display,win,gc,x,y);
   if(buffered)
   	XDrawPoint(display,cont_pixmap,gc,x,y);
   }
}


void fill_rect(int x1,int y1,int x2,int y2,unsigned  char c)
{
x1*=mulx; /* Multiplicaton of coordinates */
y1*=muly; /* if window is bigger */
x2*=mulx; /* Multiplicaton of 2' coordinates */
y2*=muly; /* if window is bigger */

if(c!=CurrForeground)
   { 
   CurrForeground=c;
   XSetForeground(display,gc,Scale[c]);
   }

if(!animate)
   	XFillRectangle(display, win, gc, x1, y1, x2-x1, y2-y1 ); 
if(buffered)
   	XFillRectangle(display, cont_pixmap, gc, x1, y1, x2-x1, y2-y1 ); 
 
}

void fill_poly(int vx,int vy,
					const ssh_point points[],int number,
					unsigned  char c)
/* Wypelnia wielokat przesuniety o vx,vy w kolorze c */
{
static XPoint _LocalTable[10];
XPoint* 	LocalPoints=_LocalTable;
int i;

if(number<=2)
		return; /*Nie da sie rysowac 
		wielokata o dwu punktach lub
			mniej*/

if(number>10) /*Jest za duzy.Alokacja*/
	LocalPoints=calloc(number,sizeof(XPoint));

if(LocalPoints==NULL)
		 {assert("Memory for polygon");return;}

if(c!=CurrForeground)
   { 
   CurrForeground=c;
   XSetForeground(display,gc,Scale[c]);
   }
   
vx*=mulx;
vy*=muly;

for(i=0;i<number;i++)
	{
	LocalPoints[i].x=points[i].x*mulx+vx;
	LocalPoints[i].y=points[i].y*muly+vy;
	}

if(!animate)
   	XFillPolygon(display, win, gc,
   	LocalPoints,number,Complex,CoordModeOrigin); 
if(buffered)
   	XFillPolygon(display, cont_pixmap, gc,
   	 LocalPoints,number,Complex,CoordModeOrigin); 

if(number>10) /*Byl duzy*/
	free(LocalPoints);
}

void line(int x1,int y1,int x2,int y2,unsigned  char c)
/* wyswietlenie lini */
{
static unsigned line_width=0;
x1*=mulx;x2*=mulx; /* Multiplicaton of coordinates */
y1*=muly;y2*=muly;  /* if window is bigger */
if(c!=CurrForeground)
   { 
   CurrForeground=c;
   XSetForeground(display,gc,Scale[c]);
   }
if( line_width!=(mulx>muly?muly:mulx) )
	{
	line_width=(mulx>muly?muly:mulx);
	if(trace)
		{
		printf("Set line width to %d\n",line_width);
		}
	XSetLineAttributes(display, gc,line_width,
		 LineSolid,  CapRound, JoinRound);
	}
if(!animate)
	XDrawLine(display,win, gc, x1, y1, x2, y2);
if(buffered)
	XDrawLine(display,cont_pixmap, gc, x1, y1, x2, y2);
}

void clear_screen()
/* Czysci ekran przed zmiana zawartosci */
{  
XSetForeground(display, gc, Scale[bacground] );
CurrForeground=-1; 
/* Clear screen and bitmap */
if(!animate)
	{
	if(trace) printf("Clear window\n");
    	XFillRectangle(display,win , gc, 0,0,width,height);
	}
if(buffered)
	{
	if(trace) printf("Clear pixmap\n");
	XFillRectangle(display,cont_pixmap , gc, 0,0,width,height);
	}
}

void circle(int x,int y,int r,unsigned char c)
/* Wyswietlenie okregu w kolorze c */
{
int angle2=360*64,r1,r2;
x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
r1=r*mulx;r2=r*muly;  /* if window is bigger */
if(c!=CurrForeground)
   { 
   CurrForeground=c;
   XSetForeground(display,gc,Scale[c]);
   }
if(!animate)
	XDrawArc(display, win , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
if(buffered)
	XDrawArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
}

void fill_circle(int x,int y,int r,unsigned char c)
/* KOlo w kolorze c */
{
int angle2=360*64,r1,r2;
x*=mulx;y*=muly;   /* Multiplicaton of coordinates */
r1=r*mulx;r2=r*muly;  /* if window is bigger */
if(c!=CurrForeground)
   { 
   CurrForeground=c;
   XSetForeground(display,gc,Scale[c]);
   }
if(!animate)
	XFillArc(display, win , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
if(buffered)
	XFillArc(display, cont_pixmap , gc, x-r1, y-r2, r1*2, r2*2, 0, angle2);
}


int repaint_area(int* x,int* y,int* width,int* height)
{
if(repaint_flag==1)
	{
	*x=last_repaint_data.x/mulx;
	if(*x<0) *x=0;
	if(*x>org_width) *x=org_width;

	*y=last_repaint_data.y/muly;
	if(*y<0) *y=0;
	if(*y>org_height) *y=org_height;

	*width=last_repaint_data.width/mulx;
	if(*x+*width<0) *width=0;
	if(*x+*width>org_width) *width=org_width-*x;

	*height=last_repaint_data.height/muly;
	if(*y+*height <0) *height =0;
	if(*y+*height>org_height) *height =org_height-*y;
	repaint_flag=0;
	
	return 0;
	}
	else
	return -1;	
}

int  get_mouse_event(int* xpos,int* ypos,int* click)
/* Odczytuje ostatnie zdazenie myszy */
{
if(LastMouse.flags!=0)
	{
	*xpos=LastMouse.x/mulx;
	*ypos=LastMouse.y/muly;
	*click=LastMouse.buttons;
	LastMouse.flags=0;
	return 0;
	}
return -1;
}

static void SetScale(XColor RGBarray[])
{
unsigned k;
XColor RGB;

RGB.red=0;RGB.green=0;RGB.blue=0;

for(k=0;k<256;k++)
    {
    RGBarray[k]=RGB;
    }


if(UseGrayScale)
  {
  for(k=0;k<255;k++)
    { 
    long wal=k;
    /*printf("%u %ul\n",k,wal);*/
    RGBarray[k].red=wal;
    RGBarray[k].green=wal;
    RGBarray[k].blue=wal;
    }      
  }
  else
  {
  for(k=0;k<255;k++)
	{
	long wal;
	double kat=(M_PI*2)*k/255.;
	wal=255*sin(kat*1.25);
	if(wal>0)  RGBarray[k].red=wal;
	wal=255*(-sin(kat*0.85));
	if(wal>0)  RGBarray[k].green=wal;
	wal=255*(-cos(kat*1.1));
	if(wal>0)  RGBarray[k].blue=wal;
	}
   }

RGBarray[255].red=0xffff;
RGBarray[255].green=0xffff;
RGBarray[255].blue=0xffff;

if(trace)
	printf("%s\n","SetScale completed");
}

void set_rgb(int color,int r,int g,int b)
/* Zmienia definicja koloru. Indeksy 0..255 */
{
XColor pom;
unsigned long pixels[1];
pom.red=r*256;
pom.blue=b*256;
pom.green=g*256;
pixels[0]/*=pom.pixel*/=Scale[color];
pom.pixel=0;	
if(XAllocColor(display,colormap,&pom)!=0)
    {
    Scale[color]=pom.pixel;	
    /*Niepewna metoda,lub wrecz zla */
    XFreeColors(display,colormap,pixels, 1,0);
    }
}


/*#pragma exit close_plot*/


