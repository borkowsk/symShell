///////////////////////////////////////////////////////////////////////////////////////////
//			Prosty program SYMSHELLA implememtuj¹cy moder Solomona
//-----------------------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////////////////
#define USES_ASSERT
#define USES_STDIO
#define USES_STDLIB
#define USES_STRING
#define USES_SYMULATION_SHELL /* rownowazne #include "symshell.h" */

#include "wb_uses.h"
#include "wb_ptr.hpp"
#include "wb_rand.hpp"

typedef double my_type;
bool losuj=true;
unsigned long ile_krokow=0;
unsigned long ile_monte_carlo=0;

size_t bok=256;
my_type min=0;
my_type max=2;
int  x=TheRandG.Random(bok);
int  y=TheRandG.Random(bok);

wb_dynmatrix<my_type> Swiat(bok,bok);

void replot();//Repaint all screen - prymitywne ale skuteczne
void status();

void init()
{
    print(screen_width()/2,screen_height()/2,"Inicjalizacja...");
    if(losuj)
    {
        for(int i=0;i<bok; i++)
            for(int j=0;j<bok; j++)
            {
                Swiat[i][j]=TheRandG.Random(3);
            }
    }
    else
    {
        Swiat.fill(1);
    }
}

inline unsigned char color(my_type kom)
{
    return kom;
}

void one_step()
{
    static long monte_step=long(bok*bok);

    unsigned char pom=color(Swiat[x][y]);
    plot(x,y,pom);//Wyswietlenie starej pozycji

    int i=TheRandG.Random(3)-1;
    int j=TheRandG.Random(3)-1;
    i+=x;
    j+=y;
    i%=bok;
    j%=bok;   
    plot(i,j,255);//Wyswietlenie badanej pozycji
 
    Swiat[i][j]=TheRandG.Random(50);
    x=i;
    y=j;

    ile_krokow++;
    if((ile_krokow%monte_step)==0)
    {
        ile_monte_carlo++;
        status();
    }
}

void status()
{
    //Linia statusu
    print(0,screen_height()-char_height('X'),"%lu k.m-c=%lu k.agenta",ile_monte_carlo,ile_krokow);      
}


void replot()//Repaint all screen - prymitywne ale skuteczne
{
    unsigned int i,j;
    unsigned char pom;
    int old=mouse_activity(0);//Ewentualna dezaktywacja myszy na czas odrysowywania
    clear_screen();
    
    //Odrysowywanie "bitmapy"
    for(i=0;i<bok; i++)
    {
        for(j=0;j<bok; j++)
        {
            pom=color(Swiat[i][j]);
            plot(i,j,pom);
        }
    }

    //Odrysowywanie skali
    int start=bok+5;
    for(int c=0;c<256;c++)
        fill_rect(start,c,start+10,c+1,c);
    
    //Odrysowywanie reszty    
    status();
    //print(screen_width()-char_width('X'),screen_height()-char_height('X'),"X");
    
    flush_plot();		//Koniec rysowania
    mouse_activity(old);//Ewentualna aktywacja myszy
}

/*  OGOLNA FUNKCJA MAIN */
/************************/

main(int argc,char* argv[])
{
    int i=0,xpos=0,ypos=0,click=0;//Myszowate -na razie niepotrzebne
    int cont=1;//flaga kontynuacji
    int std=0;
    
    //INICJACJA APLIKACJI
    mouse_activity(0);
    set_background(128);
    buffering_setup(0);/* Wylaczona animacja - tu niepotrzebna */
    shell_setup("SORIN MODEL DEMONSTRATION",argc,argv);
    printf("SORIN MODEL DEMONSTRATION %dx%d\n",bok,bok);
    /*
    printf("COLORS=256 q-quit\n"
        "setup options:\n"
        " -mapped -buffered -traceevt\n"
        " + for enable\n - for disable\n ! - for check\n");*/
    
    if(!init_plot(256,256,0,1))
    {
        printf("%s\n","Can't initialize graphics");
        exit(1);
    }

    init();
    replot();

    //PETLA GLOWNA
    while(cont)
    {
        int tab;
        
        if(input_ready())//Jesli nie ma wejscia to robi swoje 
        {
            tab=get_char();//Jest wejscie. Nie powinien sie tu zatrzymac. Odczytuje.
            switch(tab)
            {
            case '@':					//Reczne odpalanie replot()
            case '\r':replot();break;	//Systemowe odpalanie replot()
            //case '\b':get_mouse_event(&xpos,&ypos,&click);//Obsluga zdarzenia myszy
            //    if(click&0x1) 
            //        set_char('X');
            //    break;            
            case 'q':					//Reczne zakonczenie aplikacji
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
    printf("\nDo widzenia!!!\n"); 
    return 0;
}

int WB_error_enter_before_clean=1;// Wait for user in close_plot()