///////////////////////////////////////////////////////////////////////////////////////////
//							Przykladowy program SYMSHELLA
//-----------------------------------------------------------------------------------------
//Demonstruje skalowanie okna niezalezne od aplikacyjnej strony programu
//i robione w funkcjach symshella. Petla oblsugi zdarzen bez pracy w tle
///////////////////////////////////////////////////////////////////////////////////////////
#define USES_ASSERT
#define USES_STDIO
#define USES_STDLIB
#define USES_STRING
#define USES_SYMULATION_SHELL
#include "Wb_uses.h"
#define USES_STDC_RAND
#include "../../INCLUDE/Random.hpp"

int x,y,vx,vy;

/* For close_plot() */
int WB_error_enter_before_clean=0;/* Nie czeka z zamykaniem okna */

void replot()
{
char bufor[128];
int x,y;
int width,height;
int ow,ot;			/* OLD STATES */
ssh_color col=RANDOM(256);
repaint_area(&x,&y,&width,&height);/* Podaje obszar ktory ma byc odnowiony i zwraca 0 */
fill_rect(x,y,x+width,y+height,col);

col=RANDOM(256);
width=RANDOM(5);
y=RANDOM(screen_height());
ow=line_width(width);/* Ustala szerokosc lini - moze byc kosztowne. Zwraca stan poprzedni */
line(0,y,screen_width(),y,col);

col=RANDOM(256);
sprintf(bufor,"(%dx%d)",screen_width(),screen_height());
ot=print_transparently(1);/* Wlacza drukowanie tekstu bez zamazywania t³a. Zwraca stan poprzedni */
printc(screen_width()/2-string_width(bufor)/2,screen_height()/2,
	   col,0,"%s",bufor);

flush_plot();
}

/*  OGOLNA FUNKCJA MAIN */
/************************/

main(int argc,char* argv[])
{
int cont=1;//flaga kontynuacji

mouse_activity(0);	/*Mysz niepotrzebna */
fix_size(0);		/* Elastyczne rozmiary okna */
set_background(0);  /* Czarne tlo */
buffering_setup(0);	/* Wylaczona animacja - tu niepotrzebna */
shell_setup("SYMSHELL - GRAPHICS EXTENDED FEATURES",argc,argv);
printf("SYMSHELL TEST:\n -help for swiches\n");

RANDOMIZE();

if(!init_plot(50,50,0,0))
		{
      printf("%s\n","Can't initialize graphics");
      exit(1);
      }


while(cont)
{
int inpt;
// Czekaj na wejscie
inpt=get_char();
switch(inpt)
   	{
    case '@':
   	case '\r':replot();break;
   	case EOF: 
		cont=0; 
		break;
   	}
}

close_plot();
printf("Do widzenia!!!\n"); 
return 0;
}
