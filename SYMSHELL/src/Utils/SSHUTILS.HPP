#ifndef __SSHUTILS_HPP__
#define __SSHUTILS_HPP__
// Symshell UTILS 
//----------------------------------------------------
const default_white=255;
const default_black=0;
const frame_width=1;

struct settings_bar3d
{
settings_bar3d(int ia=10,int ib=10,int ic=6,unsigned wcol=default_white):
		a(ia),b(ib),c(ic),wire(wcol){}
int a,b,c;
unsigned wire;
};

//KOnfiguracja slupkow 3D. Zwraca poprzednia konfiguracje.
//Jesli parametr ==NULL to przywraca poprzednio zapamietana
const settings_bar3d* bar3d_config(settings_bar3d* st);
//Rysuje slupek 3D
void bar3d(int x,int y,int h,unsigned int col1,unsigned int col2);
//Rysuje kwadratowa ramke
void rect(int x1,int y1,int x2,int y2,unsigned int frame_c);

#endif


