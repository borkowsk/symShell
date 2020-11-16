//PLIK SZABLONU ZADANIA:
//Poszukaæ w poni¿szym kodzie sekwencji TODO i wykonaæ uzupe³nienia zgodnie ze 
//znajduj¹cymi siê w tych miejscach wskazówkami.
//Przetestowany kod wys³aæ na maila:
//              x.x@uw.edu.pl 

////////////////////////////////////////////////////////////////////////////////
//			Mapa funkcji 2D - przykladowy program SYMSHELLA 
//------------------------------------------------------------------------------
// Bardzo prosty test grafiki, ale z odtwarzaniem ekranu i obslug¹ zdarzen.
// POD Dev-Cpp potrzebne s¹ dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////

#include "symshell.h"
#include <stdio.h> //Wyjœcie na konsole a la jêzyk C - printf(....)
#include <math.h>

//Wyjœciowy rozmiar "ekranu"
const int size=400;

//Do wizualizacji obs³ugi zdarzeñ
const int DELA=100;//Oczekiwanie w obrêbie pêtli
const char* CZEKAM="Klikaj! ";
//Ostatni "klik" myszy
int xmouse=10,ymouse=10; 

//Badany zakres parametrów funkcji
double Xstart=0;//-3.14;
double Xend=62.8;
double Ystart=0;//-3.14;
double Yend=62.8; 
 
void replot()
//Rysuje coœ na ekranie
{
  int xsize=screen_width();//Aktualne mog¹ byæ ró¿ne od poczatkowych!
  int ysize=screen_height()-char_height('X'); 
  
  double Xstep=(Xend-Xstart)/xsize;
  double Ystep=(Yend-Ystart)/ysize;
  printf("%g %g\n",Xstep,Ystep); //sprawdzenie na konsoli, jak w C
  //cout<<Xstep<<' '<<Ystep<<endl;//Albo bardziej w stylu C++
  
  int x,y;
  double XX,YY;
  for(x=0,XX=Xstart;XX<Xend;x++,XX+=Xstep)
    for(y=0,YY=Ystart;YY<Yend;y++,YY+=Ystep)  
      {
	    //TU WSTAWIAMY WYRAZENIE DO ZMAPOWANIA:
        //double ZZ=XX*YY; //Np. zwyk³e mno¿enie
        double ZZ=sin(sqrt(XX))*sin(pow(YY,0.5));//Albo coœ ciekawszego
		
        //TODO - powinna byæ osobna funkcja np double fxy(double x,double y);
        //Wtedy bêdzie j¹ mo¿na bezpiecznie u¿yæ zarówno w funkcji replot()
        //jak i w funkcji zapisz_do_pliku()
        
        int z=int(ZZ*256)%256; //Cala skala na zakres 0..1, nastêpne wartoœci siê powtarzaj¹
        if(z<0) z=z+256;   //"Obrocenie" ujemnych
      //  printf("%d ",z); //sprawdzenie wartoœci wydrukiem na konsoli
        plot(x,y,z);
      }
  //Ostatnie polozenie kliku - bia³y krzy¿yk   
  line(xmouse,ymouse-10,xmouse,ymouse+10,255);
  line(xmouse-10,ymouse,xmouse+10,ymouse,255);
}

void zapisz_do_pliku()
{
     //TODO - funkcja powinna zapisaæ funkcje do pliku zamiast j¹ wyrysowaywaæ na ekranie
     //Format - tabela liczb typu double rozdzielanych tabulacjami
     //Nazwa pliku na razie ustalona z góry
}

void read_mouse()
//Przykladowa procedura obslugi myszy
{ 
   int xpos,ypos,click;
   if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
   { /*
      xmouse=xpos;ymouse=ypos;
      fill_circle(xmouse,ymouse,10,254);  //TODO - zamiana kó³ka na krzy¿yk
      //TODO - wydruk wartoœci funkcji w tym punkcie na konsole                                          
      //...
	  */
   }                      
}

int main(int argc,char* argv[])//Potrzebne s¹ paramatry wywo³ania programu
{
 fix_size(0);
 mouse_activity(1); // Czy mysz bêdzie obs³ugiwana   
 buffering_setup(1);// Nie bêdzie pamietaæ w bitmapie zawartosc ekranu 
 shell_setup("WIZUALIZACJA FUNKCJI 2D",argc,argv);// Przygotowanie okna  

 init_plot(size,size,0,1);/* Otwarcie okna SIZExSIZE pixeli + 1 wiersz znakow za pikselami*/

 /* Teraz mo¿na rysowaæ i pisac w oknie */
 replot();
 flush_plot();	/* Obraz/bitmapa gotowa */

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
       case 'p': zapisz_do_pliku();break;//TODO - tê fukncje trzeba powy¿ej wype³niæ treœci¹
       //TODO case 'x': //pyta o startow¹ wartoœæ X
       //TODO case 'X': //pyta o koñcow¹ wartoœæ X
       //TODO case 'y': //pyta o startow¹ wartoœæ Y
       //TODO case 'Y': //pyta o koñcow¹ wartoœæ Y
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
       if(loop%2==0)
        printc(0,screen_height()-char_height('C'),128,255,CZEKAM);
       else
        printc(0,screen_height()-char_height('C'),255,128,CZEKAM);
    
       flush_plot();// gotowe  - i wlasciwie co chwila wywolywane 
       delay_ms(DELA);//Wymuszenie oczekiwania - ¿eby pêtla nie zjada³a ca³ego procesora  
      } 
    }
 	    
 printf("Wykonano %d obrotow petli.\nDo widzenia!\n",loop);
 close_plot();/* Zamykamy okno - po zabawie */
 printf("Do widzenia!\n");
 return 0;
}


