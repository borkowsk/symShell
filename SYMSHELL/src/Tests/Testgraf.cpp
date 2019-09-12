////////////////////////////////////////////////////////////////////////////////
//			Przykladowy program SYMSHELLA z odtwarzaniem ekranu
//------------------------------------------------------------------------------
// Bardzo prosty test grafiki z funkcj¹ odtwarzania ekranu. 
// POD Dev-Cpp potrzebne s¹ dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////
#include "symshell.h"
#include <algorithm> //bo min
#include <stdio.h>

//Rozmiar "ekranu"
const int size=400;

//Do wizualizacji obs³ugi zdarzeñ
const int DELA=100;//Oczekiwanie w obrêbie pêtli
const char* CZEKAM="Klikaj! ";
 
//Ostatni "klik" myszy
int x=10,y=10; 


void read_mouse()
//Przykladowa procedura obslugi myszy
{ 
   int xpos,ypos,click;
   if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
   {
      x=xpos;y=ypos;
      fill_circle(x,y,10,254);                                              
   }                      
}

void replot()
//Rysuje coœ na ekranie
{
  for(unsigned c=1;c<std::min(screen_width(),screen_height());c++)   
  {
     circle(0,0,c,255);
     line(size,size,c,size-2*c,c%256);//Musi byæ % (modulo) bo inaczej kolor mo¿e przekroczyæ zakres
     //delay_ms(10);
  }
  fill_circle(screen_width()/2,screen_height()/2,10,128);
  //Dodaj krzy¿yk w miejscu ostatniego klikniêcia
  //... TODO
  line(x-10,y,x+10,y,250);
  line(x,y-10,x,y+10,253);
}

int main(int argc,char* argv[])//Potrzebne s¹ paramatry wywo³ania programu
{
 fix_size(1);       // Czy emulujemy ekran o sta³ych rozmiarach?
 mouse_activity(1); // Czy mysz bêdzie obs³ugiwana   
 buffering_setup(0);// Nie bêdzie pamietaæ w bitmapie zawartosc ekranu 
 shell_setup("VERY SIMPLE TEST 2",argc,argv);// Przygotowanie okna  

 init_plot(size,size,1,1);/* Otwarcie okna SIZExSIZE pixeli + 1x1 znakow za pikselami*/

 /* Teraz mo¿na rysowaæ i pisac w oknie */
 bool kontynuuj=true;	//Zmienna steruj¹ca zakoñczeniem programu
 unsigned loop=0;
 while(kontynuuj) //PÊTLA OBS£UGI ZDARZEÑ
    { 
      int pom; //NA ZNAK Z WEJŒCIE OKNA GRAFICZNEGO
      loop++; //Zliczanie nawrotów
      if(input_ready()) //Czy jest zdarzenie do obs³ugi?
      {
       pom=get_char(); //Przeczytaj nades³any znak
       switch(pom)
       {
       case '\r': replot();break;//Wymagane odrysowanie
       case '\b': read_mouse();break;//Jest zdarzenie myszy
       case EOF:  //Typowe zakoñczenie
       case  27:  //ESC
       case 'q':  //Zakoñczenie zdefiniowane przez programiste
       case 'Q': kontynuuj=false;break;             
       default:
            print(0,screen_height()-char_height('N'),"Nie wiem co znaczy %c [%d] ",pom,pom);
            printf("Nie wiem co znaczy %c [%d] ",pom,pom);
            flush_plot();	// Bitmapa gotowa   
            break;
        }
      }
      else //Akcja na wypadek braku zdarzeñ do obslugi
      {
       printc(0,screen_height()-char_height('C'),128,255,CZEKAM);
       flush_plot();// gotowe   
       delay_ms(DELA);//Wymuszenie oczekiwania - ¿eby pêtla nie zjada³a ca³ego procesora  
      } 
    }
 	    
 printf("Wykonano %d obrotow petli.\nDo widzenia!\n",loop);
 close_plot();/* Zamykamy okno - po zabawie */
 printf("Do widzenia!\n");
 return 0;
}


