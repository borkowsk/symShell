///////////////////////////////////////////////////////////////////////////////////////////
//	  Prosty program SYMSHELLA implememtuj¹cy model Solomona i wariacje na temat
//-----------------------------------------------------------------------------------------
//    - s¹ to przyk³ady probablistycznych turmitów (krewniaków Mrówki Langtona)
///////////////////////////////////////////////////////////////////////////////////////////
#define USES_ASSERT
#define USES_SIGNAL
#define USES_STDIO
#define USES_STDLIB
#define USES_STRING
#include "INCLUDE/uses_wb.hpp"

#include "INCLUDE/wb_ptr.hpp"
#include "INCLUDE/wb_rand.hpp"
#include "SYMSHELL/symshell.h"
#include <setjmp.h>

using namespace wbrtm;


#define TITLE_MAIN "SORIN FAMILY OF MODELS v. 1.06 (by Wojciech Borkowski)"

#define TITLE0 "RANDOM JUMP PSEUDO-SORIN MODEL"
#define FUN0   one_step_random_jump

#define TITLE1 "RANDOM WALK PSEUDO-SORIN MODEL"
#define FUN1   one_step_random_walk

#define TITLE2 "RANDOM CHANGE PSEUDO-SORIN MODEL"
#define FUN2   one_step_random_change

#define TITLE3 "PLUS-MINUS SORIN MODEL"
#define FUN3   one_step_plus_minus

#define TITLE4 "WITHOUT TABU PSEUDO-SORIN MODEL"
#define FUN4   one_step_without_tabu

#define TITLE5 "CLASSICAL SORIN MODEL"
#define FUN5   one_step_sorin

typedef double my_type;
typedef unsigned long my_count;
bool czysc=true;
bool losuj=true;
unsigned long ile_krokow=0;
unsigned long ile_monte_carlo=0;

#ifdef STD_C_RND
RandSTDC TheRndGen;//Standardowy generator jezyka C
#else
RandG TheRndGen;//Generator z Numerical Recipes
#endif

const int bok=256;//WIELKOŒC BOKU ŒWIATA

int  ox=-1; //Stare pozycje - potrzebne w algorytmie z "predkoscia"
int  oy=-1;
int  x=TheRndGen.Random(bok);
int  y=TheRndGen.Random(bok);

bool    vchenged=false;
my_type vmin=DBL_MAX; //Minimum dla wartosci
my_type vmax=-DBL_MAX;//Maksimum dla wartosci
double DzielnikSkaliV=((((((1/2.0)/2.0)/2.0)/2.0)/2.0)/2.0);

const my_count  cmin=0; //Stale minimum dla czestosci
my_count  cmax=1; //Maximum dla czestosci
double DzielnikSkaliC=((((((1/2.0)/2.0)/2.0)/2.0)/2.0)/2.0);

wb_dynmatrix<my_type>       Swiat;//Swiat symulacji
wb_dynmatrix<my_count>      Odwiedziny;//Swiat symulacji
wb_dynarray<unsigned long>  LicznikiV; //Liczniki w poszczegolnych przedzialach kolorów
wb_dynarray<unsigned long>  LicznikiC; //Liczniki w poszczegolnych przedzialach kolorów

void full_replot();//Odrysowuje wszystko - prymitywne ale skuteczne
void vals_replot();//Odrysowuje stronê wartoœci
void freq_replot();//Odrysowuje stronê frekwencji
void status_replot();//Tylko status odrysowuje

inline bool sprawdz_skale(double min,double max,double& DzielnikSkali,int jeden)
{
    double pmin=0;
    bool ret=false;
    bool ok;
    
    do{
        ok=true;//Jesli skala jest (juz) OK to to zostaje true
        if(min>0)
            pmin=-log10(min);

        if(pmin/DzielnikSkali>jeden)
        {
            DzielnikSkali*=2;
            ret=true;
            ok=false;
        }
        
        double pmin=log10(max);
        if(pmin/DzielnikSkali>255-jeden)
        {
            DzielnikSkali*=2;
            ret=true;
            ok=false;
        }
    } while(!ok);//Wiec wychodzimy z petli, a inaczej powarzamy do skutku
    
    return ret;
}

inline unsigned char color_v(double kom)
{
    if(kom==0)
    return 0;//Bo od 0 do 1 losowane
    else
    return 100+log10(kom)/DzielnikSkaliV;
}

inline unsigned char color_c(double kom)
{
    if(kom==0)
    return 0;//Bo od 0 do 1 losowane
    else
    return log10(kom)/DzielnikSkaliC;
}

void init()
{
    print(screen_width()/2,screen_height()/2,"Initialisation...");
    flush_plot();

    Swiat.alloc(bok,bok);//Swiat symulacji
    Odwiedziny.alloc(bok,bok);//Swiat symulacji
    LicznikiV.alloc(256); //Liczniki w poszczegolnych przedzialach kolorów
    LicznikiC.alloc(256); //Liczniki w poszczegolnych przedzialach kolorów

    if(losuj)
    {
        for(int i=0;i<bok; i++)
            for(int j=0;j<bok; j++)
            {
                my_type val=TheRndGen.DRand()+0.001;
                Swiat[i][j]=val;
                if(val>vmax) vmax=val;//Sprawdzamy czy max sie nie powiekszylo
                if(val<vmin) vmin=val;//... i czy min sie nie pomniejszylo
            }
    }
    else
    {
        Swiat.fill(0.5);
        vmin=0.5;vmax=1;
    }
    
    sprawdz_skale(vmin,vmax,DzielnikSkaliV,100);
    sprawdz_skale(cmin,cmax,DzielnikSkaliC,1);
    Odwiedziny.fill(0);

    clear_screen();
    flush_plot();
}

void every_monte()
{
    if( 
        sprawdz_skale(vmin,vmax,DzielnikSkaliV,100) 
        ||
        sprawdz_skale(cmin,cmax,DzielnikSkaliC,1)
        ||
        vchenged //Wymuszenie pelnego odrysowania jesli zmienilo sie maksimum
        )
    {
        full_replot();
        vchenged=false;
    }
    else
    {
        status_replot();
        freq_replot();
    }
}


void one_step_sorin()//
{
    //Zliczanie odwiedzin
    if((++Odwiedziny[x][y])>cmax)
        cmax=Odwiedziny[x][y];

    //Poprzednio uzywana pozycja zmienia stan        
    my_type val=Swiat[x][y];
    if(TheRndGen.DRand()<0.5)
       val/=2; 
    else 
       val*=2;
    if(val>vmax) {vmax=val;vchenged=true;}//Sprawdzamy czy max sie nie powiekszylo
    if(val<vmin) {vmin=val;vchenged=true;}//... i czy min sie nie pomniejszylo
          
    //Wyswietlenie starej pozycji
    Swiat[x][y]=val;
    if(czysc) plot(x,y,color_v(val));

    //Decyzja o kroku który wykonujemy na nowa pozycje
    struct {int nx,ny;} dirs[4]={ // <-- Tablica nowych pozycji do wyboru
                                {(x-1+bok)%bok, y},
                                {(x+1)%bok,     y},
                                { x,           (y-1+bok)%bok},
                                { x,           (y+1)%bok}
                               };//Sa cztery kierunki
         
    //Szukanie wlasciwego kierunku - w niektórych algorytmach zmiany mo¿e byæ konieczne sprawdzenie s¹siadów   
    int sta=TheRndGen.Random(4);//Losowy poczatek szukania
    my_type pomv,locmax=-DBL_MAX;
    int dir=-1;
    for(int s=0;s<4;s++)
    {
        int wchdir=(s+sta)%4;
        if(dirs[wchdir].nx==ox && dirs[wchdir].ny==oy)//Poprzednia pozycja zakazana,
                     continue; //Pomijamy, inaczej predzej czy pozniej zostaje uwiêziony
        pomv=Swiat[dirs[wchdir].nx][dirs[wchdir].ny];
        if(pomv>locmax)
        {
            locmax=pomv;
            dir=wchdir;
        }
    }

    //Zmiana pozycji
    ox=x; //Zapamietanie starej
    oy=y;
    x=dirs[dir].nx;//Ustalenie nowej
    y=dirs[dir].ny;
    if(!czysc) plot(x,y,0);//Wyswietlenie decyzji


    //Obsluga zliczania kroków
    static long monte_step=long(bok*bok);
    ile_krokow++;
    if((ile_krokow%monte_step)==0)
    {
        ile_monte_carlo++;
        every_monte();
    }
}

void one_step_random_change()//Agent krazy jak w klasycznym, ale zmiany nie sa z nim zwiazane
{
    //Dowolnie wylosowana pozycja zmienia stan       
    {
        int rndx=TheRndGen.Random(bok);
        int rndy=TheRndGen.Random(bok);
        my_type val=Swiat[rndx][rndy];
        if(TheRndGen.DRand()<0.5)
            val/=2; 
        else 
            val*=2;
        if(val>vmax) {vmax=val;vchenged=true;}//Sprawdzamy czy max sie nie powiekszylo
        if(val<vmin) {vmin=val;vchenged=true;}//... i czy min sie nie pomniejszylo
        Swiat[rndx][rndy]=val;
    }

    //Zliczanie odwiedzin
    if((++Odwiedziny[x][y])>cmax)
        cmax=Odwiedziny[x][y];      
    //Wyswietlenie starej pozycji
    if(czysc) 
    {
        my_type val=Swiat[x][y];
        plot(x,y,color_v(val));
    }

    //Decyzja o kroku który wykonujemy na nowa pozycje - w niektórych algorytmach zmiany mo¿e byæ konieczne sprawdzenie s¹siadów   
    struct {int nx,ny;} dirs[4]={ // <-- Tablica nowych pozycji do wyboru
                                {(x-1+bok)%bok, y},
                                {(x+1)%bok,     y},
                                { x,           (y-1+bok)%bok},
                                { x,           (y+1)%bok}
                               };//Sa cztery kierunki

         
    //Szukanie wlasciwego kierunku    
    int sta=TheRndGen.Random(4);//Losowy poczatek szukania
    my_type pomv,locmax=-DBL_MAX;
    int dir=-1;
    for(int s=0;s<4;s++)
    {
        int wchdir=(s+sta)%4;
        if(dirs[wchdir].nx==ox && dirs[wchdir].ny==oy)//Poprzednia pozycja zakazana,
                     continue; //Pomijamy, inaczej predzej czy pozniej zostaje uwiêziony
        pomv=Swiat[dirs[wchdir].nx][dirs[wchdir].ny];
        if(pomv>locmax)
        {
            locmax=pomv;
            dir=wchdir;
        }
    }

    //Zmiana pozycji
    ox=x; //Zapamietanie starej
    oy=y;
    x=dirs[dir].nx;//Ustalenie nowej
    y=dirs[dir].ny;
    if(!czysc) plot(x,y,(ile_krokow%2==0?254:64));//Wyswietlenie decyzji


    //Obsluga zliczania kroków
    static long monte_step=long(bok*bok);
    ile_krokow++;
    if((ile_krokow%monte_step)==0)
    {
        ile_monte_carlo++;
        every_monte();
    }
}

void one_step_plus_minus()
{
      //Zliczanie odwiedzin
    if((++Odwiedziny[x][y])>cmax)
        cmax=Odwiedziny[x][y];

    //Poprzednio uzywana pozycja zmienia stan        
    my_type val=Swiat[x][y];
    if(TheRndGen.DRand()<0.5)
    {
       val+=1;        
    }
    else 
    {
       val-=1;
        if(val<=0)
           val=0;
    }
    if(val>vmax) {vmax=val;vchenged=true;}//Sprawdzamy czy max sie nie powiekszylo
    if(val<vmin) {vmin=val;vchenged=true;}//... i czy min sie nie pomniejszylo
          
    //Wyswietlenie starej pozycji
    Swiat[x][y]=val;
    if(czysc) plot(x,y,color_v(val));

    //Decyzja o kroku który wykonujemy na nowa pozycje
    struct {int nx,ny;} dirs[4]={ // <-- Tablica nowych pozycji do wyboru
                                {(x-1+bok)%bok, y},
                                {(x+1)%bok,     y},
                                { x,           (y-1+bok)%bok},
                                { x,           (y+1)%bok}
                               };//Sa cztery kierunki

         
    //Szukanie wlasciwego kierunku - w niektórych algorytmach zmiany mo¿e byæ konieczne sprawdzenie s¹siadów      
    int sta=TheRndGen.Random(4);//Losowy poczatek szukania
    my_type pomv,locmax=-DBL_MAX;
    int dir=-1;
    for(int s=0;s<4;s++)
    {
        int wchdir=(s+sta)%4;
        if(dirs[wchdir].nx==ox && dirs[wchdir].ny==oy)//Poprzednia pozycja zakazana,
                     continue; //Pomijamy, inaczej predzej czy pozniej zostaje uwiêziony
        pomv=Swiat[dirs[wchdir].nx][dirs[wchdir].ny];
        if(pomv>locmax)
        {
            locmax=pomv;
            dir=wchdir;
        }
    }

    //Zmiana pozycji
    ox=x; //Zapamietanie starej
    oy=y;
    x=dirs[dir].nx;//Ustalenie nowej
    y=dirs[dir].ny;
    if(!czysc) plot(x,y,254);//Wyswietlenie decyzji


    //Obsluga zliczania kroków
    static long monte_step=long(bok*bok);
    ile_krokow++;
    if((ile_krokow%monte_step)==0)
    {
        ile_monte_carlo++;
        every_monte();
    }
}


void one_step_without_tabu()
{
     //Zliczanie odwiedzin
    if((++Odwiedziny[x][y])>cmax)
        cmax=Odwiedziny[x][y];

    //Poprzednio uzywana pozycja zmienia stan        
    my_type val=Swiat[x][y];
    if(TheRndGen.DRand()<0.5)
       val/=2; 
    else 
	{
		if(val<DBL_MAX/2)
			val*=2;
		else val=DBL_MAX;
	}
    if(val>vmax) {vmax=val;vchenged=true;}//Sprawdzamy czy max sie nie powiekszylo
    if(val<vmin) {vmin=val;vchenged=true;}//... i czy min sie nie pomniejszylo
          
    //Wyswietlenie starej pozycji
    Swiat[x][y]=val;
    if(czysc) plot(x,y,color_v(val));

    //Decyzja o kroku który wykonujemy na nowa pozycje
    struct {int nx,ny;} dirs[4]={ // <-- Tablica nowych pozycji do wyboru
                                {(x-1+bok)%bok, y},
                                {(x+1)%bok,     y},
                                { x,           (y-1+bok)%bok},
                                { x,           (y+1)%bok}
                               };//Sa cztery kierunki
    
    //Szukanie wlasciwego kierunku  - w niektórych algorytmach zmiany mo¿e byæ konieczne sprawdzenie s¹siadów   
    int sta=TheRndGen.Random(4);//Losowy poczatek szukania
    my_type pomv,locmax=-DBL_MAX;
    int dir=-1;
    for(int s=0;s<4;s++)
    {
        int wchdir=(s+sta)%4;
        pomv=Swiat[dirs[wchdir].nx][dirs[wchdir].ny];
        if(pomv>locmax)
        {
            locmax=pomv;
            dir=wchdir;
        }
    }
    //Zmiana pozycji
    x=dirs[dir].nx;
    y=dirs[dir].ny;
    if(!czysc) plot(x,y,256+50);//Wyswietlenie decyzji

    //Zliczanie krokow
    static long monte_step=long(bok*bok);
    ile_krokow++;
    if((ile_krokow%monte_step)==0)
    {
        ile_monte_carlo++;
        every_monte();
    }
}


void one_step_random_walk()
{
      //Zliczanie odwiedzin
    if((++Odwiedziny[x][y])>cmax)
        cmax=Odwiedziny[x][y];

    //Poprzednio uzywana pozycja zmienia stan        
    my_type val=Swiat[x][y];
    if(TheRndGen.DRand()<0.5)
       val/=2; 
    else 
       val*=2;
    if(val>vmax) {vmax=val;vchenged=true;}//Sprawdzamy czy max sie nie powiekszylo
    if(val<vmin) {vmin=val;vchenged=true;}//... i czy min sie nie pomniejszylo
         
    //Wyswietlenie starej pozycji
    Swiat[x][y]=val;
    if(czysc) plot(x,y,color_v(val));

    //Szukanie wlasciwego kierunku  - w niektórych algorytmach zmiany mo¿e byæ konieczne sprawdzenie s¹siadów, ale nie tu!   
    //Decyzja o kroku który wykonujemy na nowa pozycje 
    int i=TheRandG.Random(3)-1;
    int j=TheRandG.Random(3)-1;
    i+=x+bok;
    j+=y+bok;
    i%=bok;
    j%=bok;   
    if(!czysc) plot(i,j,255);//Wyswietlenie badanej pozycji  
    x=i;
    y=j;

    //Zliczanie krokow
    static long monte_step=long(bok*bok);
    ile_krokow++;
    if((ile_krokow%monte_step)==0)
    {
        ile_monte_carlo++;
        every_monte();
    }
}

void one_step_random_jump()
{
      //Zliczanie odwiedzin
    if((++Odwiedziny[x][y])>cmax)
        cmax=Odwiedziny[x][y];

    //Poprzednio uzywana pozycja zmienia stan        
    my_type val=Swiat[x][y];
    if(TheRndGen.DRand()<0.5)
       val/=2; 
    else 
       val*=2;
    if(val>vmax) {vmax=val;vchenged=true;}//Sprawdzamy czy max sie nie powiekszylo
    if(val<vmin) {vmin=val;vchenged=true;}//... i czy min sie nie pomniejszylo
         
    //Wyswietlenie starej pozycji
    Swiat[x][y]=val;
    if(czysc) plot(x,y,color_v(val));

	
    //Szukanie wlasciwego kierunku  - w niektórych algorytmach zmiany mo¿e byæ konieczne sprawdzenie s¹siadów, ale nie tu!   
    //Decyzja o skoku na nowa pozycje
    int i=TheRandG.Random(bok);
    int j=TheRandG.Random(bok);
    if(!czysc) plot(i,j,128+255);//Wyswietlenie badanej pozycji  
    x=i;
    y=j;

    //Zliczanie krokow
    static long monte_step=long(bok*bok);
    ile_krokow++;
    if((ile_krokow%monte_step)==0)
    {
        ile_monte_carlo++;
        every_monte();
    }
}


void plot_values()
{
    sprawdz_skale(vmin,vmax,DzielnikSkaliV,100);
    LicznikiV.fill(0);//Zerowanie liczników

    //Odrysowywanie "bitmapy"
    for(int i=0;i<bok; i++)
    {
        for(int j=0;j<bok; j++)
        {
            int pom=color_v(Swiat[i][j]); assert(pom<256);
            LicznikiV[pom]++;
            plot(i,j,pom);
        }
    }
}

void plot_freq(int startx)
{
    sprawdz_skale(cmin,cmax,DzielnikSkaliC,1);
    LicznikiC.fill(0);//Zerowanie liczników

    //Odrysowywanie "bitmapy"
    for(int i=0;i<bok; i++)
    {
        for(int j=0;j<bok; j++)
        {
            int pom=color_c(Odwiedziny[i][j]); assert(pom<256);
            LicznikiC[pom]++;
            plot(startx+i,j,pom);
        }
    }
}

void plot_scale(int startx,int starty,int size,wb_dynarray<unsigned long>& Liczniki,unsigned char (*color)(double),double min,double max)
{
    //Szukanie najczestrzej klasy
    unsigned long maxlicz=0;
    for(int l=0;l<256;l++)
    {
        if(Liczniki[l]>maxlicz)
            maxlicz=Liczniki[l];
    }
    
    //Odrysowywanie skali
    for(int c=0;c<256;c++)
    {
        double Udzial=double(Liczniki[c])/double(maxlicz); assert(Udzial<=1);
        Udzial*=size;                                                        
        int L=size-int(Udzial);
        int R=256*(Udzial-int(Udzial)); //Odcienie szarosci
        fill_rect(startx+c,starty,startx+c+1,starty+L,c);
        if(L>=0)       
            fill_rect(startx+c,starty+L,startx+c+1,starty+size,255);
        if(R>0)
            plot(startx+c,starty+L-1,255+R);       
    }

    //Opisywanie skali    
    int Height=char_height('X');
    print_transparently(true);
    fill_rect(startx,starty+size,startx+256,starty+size+4*Height+1,256+50);
    line(startx+color(min),starty+size,startx+color(min),starty+size+Height,color(min));
    line(startx+color(1),starty+size,startx+color(1),starty+size+2*Height,color(1));
    line(startx+color(max),starty+size,startx+color(max),starty+size+3*Height,color(max));

    printc(startx+color(min),starty+size+Height,color(min),0,"log10(%g)",min);   
    printc(startx+color(1),starty+size+2*Height,color(1),0,"log10(1)"); 
    wb_pchar maxstr(128);
    maxstr.prn("log10(%g)",max);
    int lenght=string_width(maxstr.get());
    if(startx+color(max)+lenght<startx+256)
        printc(startx+color(max),starty+size+3*Height,color(max),256+50,"%s",maxstr.get());        
    else
        printc(startx+color(max)-lenght,starty+size+3*Height,color(max),256+50,"%s",maxstr.get());
    print_transparently(false);
}

void status_replot()
{
    //Linia statusu 
    int height=char_height('X');
    fill_rect(0,screen_height()-2*height,bok,screen_height()-height+1,255);
    print(0,screen_height()-2*height," values, scale*%g ",DzielnikSkaliV);
    fill_rect(bok+10,screen_height()-2*height,screen_width()+1,screen_height()-height+1,255);
    print(bok+10,screen_height()-2*height," frequencies, scale*%g  ",DzielnikSkaliC);  
    fill_rect(0,screen_height()-height,screen_width()+1,screen_height()+1,255+200);
    printc(0,screen_height()-height,255+55,255+200,"%lu monte-carlo's == %lu agent's  STEPS",ile_monte_carlo,ile_krokow);    
    const char* Helpik="c-clean on/off ENTER-replot";
    int lengh=string_width(Helpik);
    printc(screen_width()-lengh,screen_height()-height,50,255+100,"%s",Helpik);
}   

void vals_replot()
{
plot_values();
plot_scale(0,bok+10,18,LicznikiV,color_v,vmin,vmax); 
flush_plot();		//Koniec rysowania
}

void freq_replot()
{
plot_freq(bok+10);
plot_scale(bok+10,bok+10,18,LicznikiC,color_c,cmin,cmax); 
flush_plot();		//Koniec rysowania
}

void full_replot()//Repaint all screen - prymitywne ale skuteczne
{
    int old=mouse_activity(0);//Ewentualna dezaktywacja myszy na czas odrysowywania
    //clear_screen();//Niepotrzebne
    fill_rect(bok,0,bok+10,screen_height(),0);

    status_replot();
    plot_freq(bok+10);
    plot_scale(bok+10,bok+10,18,LicznikiC,color_c,cmin,cmax);
    
    plot_values();
    plot_scale(0,bok+10,18,LicznikiV,color_v,vmin,vmax); 
    

    flush_plot();		//Koniec rysowania
    mouse_activity(old);//Ewentualna aktywacja myszy
}

void (*one_step)();//Wskaznik do funkcji kroku
char* Title=TITLE_MAIN;

void model_select()
{
    int sel=0;
    printf("%s","Please select a model\n\n");
    printf("0-%s\n",TITLE0);
    printf("1-%s\n",TITLE1);
    printf("2-%s\n",TITLE2);
    printf("3-%s\n",TITLE3);
    printf("4-%s\n",TITLE4);
    printf("5-%s\n",TITLE5);
    printf("?");
    scanf("%d",&sel);
    switch(sel){
    case 0:one_step=FUN0;Title=TITLE0;break;
    case 1:one_step=FUN1;Title=TITLE1;break;
    case 2:one_step=FUN2;Title=TITLE2;break;
    case 3:one_step=FUN3;Title=TITLE3;break;
    case 4:one_step=FUN4;Title=TITLE4;break;
    case 5:
    default:one_step=FUN5;Title=TITLE5;break;
    }
    printf("\n");
}

// Do wylapywania bledow numerycznych 
//**************************************
jmp_buf mark;              // Address for long jump to jump to 
int     fperr;             // Global error number 
extern "C"{
void __cdecl fphandler( int sig, int num );   /* Prototypes */
void fpcheck( void );
}

/*  OGOLNA FUNKCJA MAIN */
/************************/

int main(int argc,const char* argv[])
{
    int i=0,xpos=0,ypos=0,click=0;//Myszowate -na razie niepotrzebne
    int cont=1;//flaga kontynuacji
    int std=0;
    printf("%s\n\n",Title);
    model_select();
    printf("\nSelected model: %s %dx%d\n\n",Title,bok,bok);
    
	//INICJACJA APLIKACJI    
    TheRndGen.Reset();   
    mouse_activity(0);
    set_background(0);
    buffering_setup(0);/* Wylaczona animacja - tu niepotrzebna */
    shell_setup(Title,argc,argv);
    if(!init_plot(bok*2+10,bok+30,0,6))
    {
        printf("%s\n","Can't initialize graphics");
        exit(1);
    }

    printf("c - clean or not the agent path\n");
    printf("ENTER - replot completely\n");
     
    // Unmask floating-point exceptions. 
    //_controlfp( 0, _MCW_EM ); 
    _controlfp( _EM_INEXACT,_MCW_EM );//Trzeba wylaczyc inexact bo jest niemal wszedzie!
    //_controlfp( _EM_OVERFLOW+_EM_UNDERFLOW,_MCW_EM );

   /* Set up floating-point error handler. Some compilers
    * may generate an error because they expects
    * signal-handling functions to take only one argument.
    */
#ifdef _NDEBUG 
    if( signal( SIGFPE, (void (__cdecl *)(int))fphandler ) == SIG_ERR )
    {
        fprintf( stderr, "Couldn't set SIGFPE\n" );
        abort();   
    }
#endif

    // others inits
    init();
    full_replot();

    //PETLA GLOWNA
    while(cont)
    {
        //double t=sqrt(-1);//log10(0);// FP excp. test
        int tab; //Wczytany znak z wejscia okna graficznego
        
        if(i%1000==0 && input_ready())//Jesli nie ma wejscia to robi swoje 
        {
            tab=get_char();//Jest wejscie. Nie powinien sie tu zatrzymac. Odczytuje.
            switch(tab)
            {
            case 'c':full_replot();czysc=!czysc;break;//Czyszczenie lub nie sladow
            case '@':					//Reczne odpalanie replot()
            case '\r':full_replot();break;	//Systemowe odpalanie replot()
            //case '\b':get_mouse_event(&xpos,&ypos,&click);//Obsluga zdarzenia myszy
            //    if(click&0x1) 
            //        set_char('X');
            //    break;
            //case 27:                  //Niebezpieczne reczne zakonczenie aplikacji
            //case 'q':					//Reczne zakonczenie aplikacji
            case EOF:					//Systemowe zakonczenie aplikacji (Close w MSWin, Ctrl-C wX11)
                cont=0; 
                break;
            }
                        
            if(std)//Implementacja echa
            {
                printf("stdout<<%c\n",tab);
                fflush(stdout);
            }
            
            printc(screen_width()-char_width('X'),
                screen_height()-char_height('X'),
                34,255,"%c",tab);
        }
        
        i++;
        one_step();  
        flush_plot();
    }
    
    //Zakonczenie dzialania aplikacji
    close_plot();
    printf("\nBye!!!\n"); 
    return 0;
}

int WB_error_enter_before_clean=1;// Wait for user in close_plot()
void fphandler( int sig, int num )
{
   /* Set global for outside check since we don't want
    * to do I/O in the handler.
    */
   fperr = num;
   /* Initialize floating-point package. */
   _fpreset();
   /* Restore calling environment and jump back to setjmp. Return 
    * -1 so that setjmp will return false for conditional test.
    */
   //longjmp( mark, -1 );
   fpcheck(); 
   exit(fperr);
}

void fpcheck( void )
{
   char fpstr[30];
   switch( fperr )
   {
   case _FPE_INVALID:
       strcpy( fpstr, "Invalid number" );
       break;
   case _FPE_OVERFLOW:
       strcpy( fpstr, "Overflow" );
       break;
   case _FPE_UNDERFLOW:
       strcpy( fpstr, "Underflow" );
       break;
   case _FPE_ZERODIVIDE:
       strcpy( fpstr, "Divide by zero" );
       break;
   case _FPE_DENORMAL:      //     0x82
       strcpy( fpstr, "Denormal" );
       break;
   case _FPE_INEXACT:       //     0x86
       strcpy( fpstr, "Inexact" );
       break;
   case _FPE_UNEMULATED:    //     0x87
       strcpy( fpstr, "Unemulated" );
       break;
   case _FPE_SQRTNEG:       //     0x88
       strcpy( fpstr, "Negative sqrt" );
       break;
   case _FPE_STACKOVERFLOW: //     0x8a
       strcpy( fpstr, "FP stack overflow" );
       break;
   case _FPE_STACKUNDERFLOW://     0x8b
       strcpy( fpstr, "FP stack underflow" );
       break;
   default:
       strcpy( fpstr, "Other floating point error" );
       break;
   }
   fprintf(stderr,"\nFloating point error %d: \"%s\"\n\a\a\a\a", fperr, fpstr );
}

   /*
    printf("COLORS=256 q-quit\n"
        "setup options:\n"
        " -mapped -buffered -traceevt\n"
        " + for enable\n - for disable\n ! - for check\n");*/


  //plot(dirs[0].nx,dirs[0].ny,255);
    //plot(dirs[1].nx,dirs[1].ny,255); //Kontrola nowych pozycji
    //plot(dirs[2].nx,dirs[2].ny,255);
    //plot(dirs[3].nx,dirs[3].ny,255);