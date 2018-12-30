//IMPLEMENTACJA DO PSEUDO BGI WRAPPERA
////////////////////////////////////////////
//Tylko to co nie moze byc "inline"
#ifdef unix
#include "pconio.h"
#else
#include <conio.h>
#endif

#include <stdio.h>
#include "SYMSHELL/pseudoBGI.h"

int	 BorlandCompat::vjustify=BorlandCompat::LEFT_TEXT;
int	 BorlandCompat::hjustify=BorlandCompat::LEFT_TEXT;
int  BorlandCompat::max_color=255;
int  BorlandCompat::my_color=255;//Kolor rysowania
int  BorlandCompat::my_fill=255;//Kolor wypelnienia
int  BorlandCompat::startx=0;//Parametr aktualnego viewport'u
int  BorlandCompat::starty=0;//Parametr aktualnego viewport'u
int  BorlandCompat::endx=0;	//Parametr aktualnego viewport'u
int  BorlandCompat::endy=0;	//Parametr aktualnego viewport'u
int  BorlandCompat::curx=0;	//aktualne x dla lineto,moveto,outtext
int  BorlandCompat::cury=0;	//aktualne y dla lineto,moveto,outtext
int  BorlandCompat::ingraph=0;//Flaga trybu graficznego

void BorlandCompat::closegraph()
	{
		BorlandCompat::cleardevice();//Czysci dla nastepnego otwarcia w tym samym programie
		close_plot();
		BorlandCompat::ingraph=0;
	}

int BorlandCompat::initgraph(double x,double y,double c/*=255*/)
	{
	//parametry double zeby roznilo sie od orginalnej
		int code=0;
		code=init_plot(x,y,0,0);

		if (code != 1)
			return 1;//Error

		BorlandCompat::ingraph=1;
		BorlandCompat::max_color=c-1;
		BorlandCompat::setcolor(BorlandCompat::max_color);
		BorlandCompat::my_fill=BorlandCompat::my_color;

		get_char();//Pierwsze '\r'
#		ifdef unix
		init_input_block_switching();
#		endif
		return 0;
	}

void BorlandCompat::clrscr()
{
	fprintf(stderr,"\n\t\f\r") ;
}

int BorlandCompat::kbhit()
{
	if(BorlandCompat::ingraph==1)
	{
		flush_plot();
		return input_ready();
	}
	else
	{
#	ifdef unix
		return input_char_ready();
#	else
		return ::kbhit();//CONIO!!!
#	endif
	}
}

int BorlandCompat::getch()
{
		if(ingraph==1)
		{
			flush_plot();
			fprintf(stderr,"EXPECTED INPUT IN GRAPHIX WINDOW\n");
			return get_char();
		}
		else
		{
#		ifdef unix
			return getchar();
#		else
			return ::getch();//CONIO!!!
#		endif
		}
}
