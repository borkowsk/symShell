#include "symshell.h"
#include "graphs.hpp"



void rect(int x1,int y1,int x2,int y2,unsigned int frame_c)
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

void bar3d(int x,int y,int h,unsigned int col1,unsigned int col2)
{
unsigned WHITE=s.wire;
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

fill_rect(x,y-h,x+s.a,y,col1);               /*front*/

fill_poly(0,0,romb+1,6,col2);              /*bok i gora*/

fill_rect(x,y,romb[1].x+1,romb[1].y+1,WHITE);       /*lewy pion*/
fill_rect(romb[1].x,romb[1].y,romb[2].x+1,romb[2].y+1,WHITE);       /*gorny poziom*/
fill_rect(x,y,romb[3].x+1,romb[3].y+1,WHITE);       /*doln poziom*/

line(romb[2].x,romb[2].y,romb[5].x,romb[5].y,WHITE); /*blik*/

//plot(romb[5].x,romb[5].y,WHITE-1);
line(romb[1].x,romb[1].y,romb[6].x,romb[6].y,WHITE);//lewy ukos
line(romb[2].x,romb[2].y,romb[3].x,romb[3].y,WHITE);//prawy ukos
line(romb[3].x,romb[3].y,romb[4].x,romb[4].y,WHITE);//dolny ukos
 
} /* end  slupek */

