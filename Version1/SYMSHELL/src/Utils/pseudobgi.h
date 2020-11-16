/********************************************************
   Symshell wrapper for most important functions of BGI 
*********************************************************/
#ifndef _PSEUDO_BGI_H_INCLUDED_
#define _PSEUDO_BGI_H_INCLUDED_

#include "symshell.h"
#include "sshutils.hpp"
#include "wb_rand.hpp"


namespace BorlandCompat
{
	const int LEFT_TEXT=0;
	const int CENTER_TEXT=1;
	const int RIGHT_TEXT=2;

	const int DOTTED_LINE=SSH_LINE_DOTTED;
	const int SOLID_LINE=SSH_LINE_SOLID;
	const int SOLID_FILL=1;
	const int grOk=0;
	const int grErr=-1;

	extern int	vjustify;//=LEFT_TEXT;
	extern int	hjustify;//=LEFT_TEXT;
	extern int  max_color;//=255;
	extern int  my_color;//=255;
	extern int  my_fill;//=255;
	extern int  startx;//=0;
	extern int  starty;//=0;
	extern int  endx;//=0;
	extern int  endy;//=0;
	extern int  curx;//=0;
	extern int  cury;//=0;
	extern int  ingraph;//=0;
	extern int  lasterr;//=0;

	inline int graphresult()
	{
		return lasterr;
	}

	const char* grapherrormsg(int errcode);

	inline int _bgi_color(int c)
	{
		if(c>0)
		{
			int my_color=256/(max_color+1);
			my_color*=c+1;
			my_color-=1;
			return my_color;
		}
		else
		{
			return 0;
		}
	}

	inline void setcolor(int c)
	{
		c=_bgi_color(c);
		my_color=c;
	}
	
	inline void cleardevice()
	{
		clear_screen();
		setcolor(max_color);
		my_fill=my_color;
		startx=0;
		starty=0;
		endx=0;
		endy=0;
		curx=0;
		cury=0;
		vjustify=LEFT_TEXT;
		hjustify=LEFT_TEXT;
		//setlinestyle(SOLID_LINE,0 /*User pattern*/,1 /*Width*/);
		line_style(SSH_LINE_SOLID);
	}

	int initgraph(double x,double y,double c=255);

	void closegraph();
	
	inline int getbkcolor()
	{
        return ::background();
	}

	inline int getmaxx()
	{
		return screen_width()-1;
	}
	
	inline int getmaxy()
	{
		return screen_height()-1;
	}
	
	inline int getmaxcolor()
	{
		return max_color;
	}	

	inline void setlinestyle(int Style,int UserPattern/*ignored*/,int Width/*ignored*/)
	{
		line_style(Style);//Zgodnosc z symshelem zapewniaja definicje stalych	
	}

	inline void setfillstyle(int shraf,int color)
	{
		my_fill=_bgi_color(color);
	}

	inline void settextjustify(int H,int V)
	{
		vjustify=V;
		hjustify=H;
	}
	
	inline void floodfill(int x,int y,int border_color)
	{
		fill_flood(x+startx,y+starty,my_fill,border_color);
	}
	
	inline void moveto(int x,int y)
	{
		curx=x;
		cury=y;
	}

	inline void setviewport(int x1,int y1,int x2,int y2,int clip)
	{
		startx=x1;
		starty=y1;
		endx=x2;
		endy=y2;
		//fill_rect(startx,starty,endx,endy,255);
		//flush_plot();
	}
	
	inline void rectangle(int x1,int y1,int x2,int y2)
	{
		rect(x1+startx,y1+starty,x2+startx,y2+starty,my_color);
	//	fill_rect(startx,starty,endx,endy,128);
	//	flush_plot();
	}

	inline void clearviewport()
	{
        fill_rect(startx,starty,endx,endy,::background());
	}
	
	inline void circle(int x,int y, int r)
	{
		::circle(x+startx,y+starty,r+1,my_color);
	}
	
	inline void line(int x1,int y1,int x2,int y2)
	{
		::line(x1+startx,y1+starty,x2+startx,y2+starty,my_color);
	}
	
	inline void fillcircle(int x,int y, int r)
	{
		::fill_circle(x+startx,y+starty,r+1,my_fill);
	}

	inline void lineto(int x2,int y2)
	{
		::line(curx+startx,cury+starty,x2+startx,y2+starty,my_color);
		curx=x2;
		cury=y2;
	}

	inline void _textjustify(int& x,int& y,const char* str)
	{
		int width=string_width(str);
		int height=string_height(str);
		switch(vjustify)
		{
		case LEFT_TEXT:
			break;
		case CENTER_TEXT:
			y-=height/2;
			break;
		case RIGHT_TEXT:
			y-=height;
			break;
		};
		switch(hjustify)
		{
		case LEFT_TEXT:
			break;
		case CENTER_TEXT:
			x-=width/2;
			break;
		case RIGHT_TEXT:
			x-=width;
			break;
		};
	}

	inline void outtextxy(int x,int y,const char* str)
	{
		int old=print_transparently(1);
		_textjustify(x,y,str);
        printc(x+startx,y+starty,my_color,::background(),"%s",str);
		print_transparently(old);
	}
	
	inline void outtext(const char* str)
	{
		int old=print_transparently(1);
		int x=curx,y=cury;
		_textjustify(x,y,str);
        printc(x+startx,y+starty,my_color,::background(),"%s",str);
		print_transparently(old);
	}

	//RANDOM
	inline void randomize()
	{
		TheRandG.Reset();
	}

	inline unsigned int random(unsigned int a)
	{
		return TheRandG.Random(a);
	}

	//CONIO
	int		kbhit();
	void	clrscr();   
	int		getch();
	/*{ //MUSI BYC W OSOBNYM MODULE Z POWODU getch
		if(ingraph)
		{
			fprintf(stderr,"EXPECTED INPUT ON GRAPHIX WINDOW\n");
			return get_char();
		}
		else
		{
			return ::getch();//CONIO!!!
		}
	}*/

};//end of namespace BorlandCompat

//USE DERECTIVE:
//	using namespace BorlandCompat;
//for correct compilation;

#endif
