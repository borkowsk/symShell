#include "symshell.h"
#include "graphs.hpp"
#include <errno.h>
#include <ctype.h>

//Drukuje w obszarze nie wiekszym niz max_width. Zwraca width albo 0
//wewnetrzny bufor ma nie wiecej niz 1024 znaki
int print_width(int x,int y,int maxwidth,wb_color col,wb_color bcg,const char* format,...)
{
char yust='L';
const size_t BUFSIZE=1024;
size_t width=0;
char bufor[BUFSIZE];
if(format[0]=='%' && format[1]=='@')//Zostal podany sposob justowania
	{
	yust=toupper(format[2]);
	format+=3;
	}
if(strchr(format,'%')!=NULL)//Sa znaki formatujace
	{
	va_list list;
	va_start(list,format);
	if(vsprintf(bufor,format,list)>=BUFSIZE)
		{
		errno=ENOMEM;
		perror("print_width internal bufor exced");
		}
	}
	else
	{
	strcpy(bufor,format);	
	}

while((width=size_t(string_width(bufor)))>maxwidth)//cast bo symshell zle zwraca
		{
		int size=strlen(bufor);
		if(size==1) break;
		bufor[size-2]='*';
		bufor[size-1]='\0';
		}
//rect(x,y,x+maxwidth,y+string_height(bufor),128);//DEBUG
if(width<=maxwidth)
	{
	switch(yust){
	case 'L':printc(x,y,col,bcg,"%s",bufor);break;
	case 'R':printc((x+maxwidth)-width,y,col,bcg,"%s",bufor);break;
	case 'C':printc(x+maxwidth/2-width/2-1,y,col,bcg,"%s",bufor);break;
	default:
		;	}
	return width;
	}
	else
	return 0;//Nie ma miejsca nawet na gwiazdke
}


void rect(int x1,int y1,int x2,int y2,wb_color frame_c)
{
fill_rect(x1,y1,x2,y1+1,frame_c);//--->
fill_rect(x2,y1,x2+1,y2,frame_c);//vvv
fill_rect(x1,y2,x2+1,y2+1,frame_c);//<---
fill_rect(x1,y1,x1+1,y2,frame_c);//^^^
}

settings_bar3d s,old;

const settings_bar3d* bar3d_config(settings_bar3d* st)
{
if(st!=NULL)
	{
	old=s;//Zapamietuje poprzednia
	s=*st;
	}
	else
	s=old;//Przywraca poprzednia
return &old;
}

void bar3d(int x,int y,int h,wb_color col1,wb_color col2)
{
wb_color WHITE=s.wire;
ssh_point romb[7];
                                       /*       6 -----  5   */
romb[1].x= x;                          /*     /        / |   */
romb[1].y= y - h;                      /*    1 ------ 2  |   */
romb[2].x= x + s.a;                    /*    |        |  |   */
romb[2].y= romb[1].y;                  /*    |        |  |   */
romb[3].x= romb[2].x;                  /*    |        |  |   */
romb[3].y= y;                          /*    |        |  4   */
romb[4].x= x + s.a + s.b;              /*    |        | /  c */
romb[4].y= y - s.c;                    /*  x,y ------ 3      */
romb[5].x= romb[4].x;                  /*         a      b   */
romb[5].y= y - h - s.c;
romb[6].x= x + s.b;
romb[6].y= romb[5].y;

fill_rect(x,y-h,x+s.a,y,col1);               //front

fill_poly(0,0,romb+1,6,col2);              //bok i gora

fill_rect(romb[1].x,romb[1].y,romb[2].x+1,romb[2].y+1,WHITE);//gorny poziom
fill_rect(x,y,romb[3].x+1,romb[3].y+1,WHITE);       //doln poziom

line(romb[2].x,romb[2].y,romb[5].x,romb[5].y,WHITE); //blik

//plot(romb[5].x,romb[5].y,WHITE-1);
line(romb[1].x,romb[1].y,romb[6].x,romb[6].y,WHITE);//lewy ukos
line(romb[2].x,romb[2].y,romb[3].x,romb[3].y,WHITE);//prawy ukos
line(romb[3].x,romb[3].y,romb[4].x,romb[4].y,WHITE);//dolny ukos
 
fill_rect(x,y-h,x+1,y+1,WHITE);       //lewy pion

} /* end  slupek */

/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/



