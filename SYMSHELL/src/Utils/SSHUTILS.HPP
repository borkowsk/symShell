#ifndef __SSHUTILS_HPP__
#define __SSHUTILS_HPP__
// Symshell UTILS 
//----------------------------------------------------
typedef unsigned wb_color;
const wb_color default_white=255;
const wb_color default_black=0;
const wb_color default_transparent=wb_color(-1);
const wb_color default_color=default_transparent;
const frame_width=1;

struct settings_bar3d
{
settings_bar3d(int ia=10,int ib=10,int ic=6,wb_color wcol=default_white):
		a(ia),b(ib),c(ic),wire(wcol){}
int a,b,c;
wb_color wire;
};

//KOnfiguracja slupkow 3D. Zwraca poprzednia konfiguracje.
//Jesli parametr ==NULL to przywraca poprzednio zapamietana
const settings_bar3d* bar3d_config(settings_bar3d* st);
//Rysuje slupek 3D
void bar3d(int x,int y,int h,wb_color col1,wb_color col2);

//Rysuje kwadratowa ramke
void rect(int x1,int y1,int x2,int y2,wb_color frame_c);

//Drukuje w obszarze nie wiekszym niz max_width. Zwraca width albo 0
//wewnetrzny bufor ma nie wiecej niz 1024 znaki
int print_width(int x,int y,int maxwidth,wb_color col,wb_color bcg,const char* format,...);


#endif


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