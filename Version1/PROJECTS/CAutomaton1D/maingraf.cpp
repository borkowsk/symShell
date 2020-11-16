////////////////////////////////////////////////////////////////////////////////
//		     	Szablon jednowymiarowego automatu komórkowego
//                   (przykladowy program SYMSHELL'a) 
//------------------------------------------------------------------------------
// Prosta obs³uga grafiki, ale z odtwarzaniem ekranu i obslug¹ zdarzen.
// POD Dev-Cpp potrzebne s¹ dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////

#include "symshell.h"
#include <stdio.h> //Wyjœcie na konsole a la jêzyk C - printf(....)
#include <math.h>
#include <fstream>
#include <iostream>

#define NAZWAMODELU  "AutomatKomorkowy1D" //U¿ycie define u³atwia montowanie sta³ych ³añcuchów
//Wyjœciowy rozmiar œwiata i "ekranu" symulacji
const int size=200;

unsigned char World[size][size];//Tablica œwiata - wyzerowana na poczatku bo globalna
         //unsigned char ¿eby by³o od 0 do 255, bo typ char bywa te¿ "signed" (zaleznie od kompilatora)
unsigned step_counter=0;//Licznik realnych kroków modelu

void init_world()
//Funkcja do zapoczatkowania œwiata
{
   const unsigned RANDOM_SEED=time(NULL); //Zarodek generatora pseudolosowego 
   srand(RANDOM_SEED); //Inicjacja generatora liczb pseudolosowych    
   for(unsigned i=0;i<size;i++)
   {
    World[0][i]=rand()%2;//To ju¿ jechanie po bandzie :-)
   } 
}

inline //Funkcja rozwijana w kodzie - oszczêdza czas wywo³ania
unsigned Regula(unsigned left,unsigned center,unsigned right)
{
   /*if(left+right==1)
     return 1;
   else
     return 0;*/
   return (left+center+right)%5;      
} 

void single_step()
//Funkcja robi¹ca jeden krok symulacji
//i wpisuj¹ca go w kolejn¹ linie
{
    unsigned old,next;
    if(step_counter<size-1)//Jest jeszcze miejsce
    {
     old=step_counter;//Poprzedni krok - indeks lini z danymi             
     next=++step_counter;//Modyfikowaæ bêdzie linia o aktualnym numerze kroku
    } 
    else //Brak miejsca
    {
     step_counter++;
     old=size-2;//Przedostatni wiersz
     next=size-1;//Ostatni wiersz
     //Pod warunkiem ¿e przesuniemy tablice
     memcpy(World,World[1],sizeof(World[0][0])*size*size-size);    
    }
    
    for(unsigned i=0;i<size;i++)//Uzycie regu³y automatu dla ka¿dej komórki nowego wiersza
        World[next][i]=Regula(World[old][(i+size-1)%size],World[old][i],World[old][(i+1)%size]);
}

//Statystyki i ich zapis do pliku
unsigned suma_komorek=0;
std::ofstream output;
void stats()
//Funkcja do obliczenia statystyk
{
  const char* NazwaPliku=NAZWAMODELU ".log";//U¿ywamy sztuczki ze zlepianiem sta³ych 
                                        //³añcuchowych przez kompilator
  if(!output.is_open()) 
  {                                     
    output.open(NazwaPliku);
    std::cout<<"Statystyki w pliku \""<<NazwaPliku<<'"'<<std::endl;
    output<<"step"<<'\t'<<"suma_komorek"<<std::endl;
  }        
  //Jakieœ atstystyki. Np. suma zawartosci komórek
  //Wystarczy ¿eby wykryæ okresowoœæ
  unsigned current;
  if(step_counter<size-1)//Jest jeszcze miejsce
    current=step_counter;
    else
    current=size-1;
    
  suma_komorek=0;
  for(unsigned i=0;i<size;i++)
        suma_komorek+=World[current][i];
  output<<step_counter<<'\t'<<suma_komorek<<std::endl;
  //Zamkniêcie pliku jak siê program skoñczy      
}


//Do wizualizacji obs³ugi zdarzeñ
const int DELA=0;//Jak d³ugie oczekiwanie w obrêbie pêtli zdarzeñ
const int VISUAL=10;//Co ile kroków symulacji odrysowywaæ widok
const char* CZEKAM="Klikaj lub patrz! "; //Monit w pêtli zdarzeñ
int xmouse=10,ymouse=10;//Pozycja ostatniego "kliku" myszy 

//Kilka deklaracja zapowiadaj¹cych inne funkcje obs³uguj¹ce model
void replot(); //Funkcja odrysowyj¹ca
void read_mouse(); // Obsluga myszy - uzywaæ o ile potrzebne
void write_to_file(); // Obsluga zapisu do pliku - u¿ywaæ o ile potrzebne
void screen_to_file(); //Zapis ekranu do pliku (tylko Windows!)

void replot()
//Rysuje coœ na ekranie
{
  for(int x=0;x<size;x++)
    for(int y=0;y<size;y++)  
      {
        unsigned z=World[y][x]*50;//Co tam by³o? Wzmocnione
        z%=256; //¯eby nie przekroczyæ kolorów
        //z%=512; //Albo z szaroœciami
        plot(x,y,z);
      }
  printc(size/2,size,128,255,"%06u  ",step_counter);//Licznik kroków    
  //Ostatnie polozenie kliku - bia³y krzy¿yk   
  //line(xmouse,ymouse-10,xmouse,ymouse+10,255);
  //line(xmouse-10,ymouse,xmouse+10,ymouse,255);
}


int main(int argc,const char* argv[])//Potrzebne s¹ paramatry wywo³ania programu
{
 fix_size(1);       // Czy udajemy ¿e ekran ma zawsze taki sam rozmiar?
 mouse_activity(0); // Czy mysz bêdzie obs³ugiwana?   
 buffering_setup(1);// Czy bêdzie pamietaæ w bitmapie zawartosc ekranu? 
 shell_setup(NAZWAMODELU,argc,argv);// Przygotowanie okna z u¿yciem parametrów wywo³ania
 init_plot(size,size,0,1);/* Otwarcie okna SIZExSIZE pixeli + 1 wiersz znakow za pikselami*/

 // Teraz mo¿na rysowaæ i pisac w oknie 
 init_world();
 replot();
 flush_plot();	// Ekran lub bitmapa po inicjalizacji jest gotowa 

 bool not_finished=true;//Zmienna steruj¹ca zakoñczeniem programu
 unsigned loop=0;    //Do zliczania nawrotów pêtli zdarzeñ
 while(not_finished) //PÊTLA OBS£UGI ZDARZEÑ
    { 
      int pom; //NA ZNAK Z WEJŒCIE OKNA GRAFICZNEGO
      loop++; 
      if(input_ready()) //Czy jest zdarzenie do obs³ugi?
      {
       pom=get_char(); //Przeczytaj nades³any znak
       switch(pom)
       {
       case 'd': screen_to_file();break; //Zrzut grafiki          
       case 'p': write_to_file();break;//Zapis do pliku tekstowego
       case '\r': replot();break;//Wymagane odrysowanie
       case '\b': read_mouse();break;//Jest zdarzenie myszy
       case EOF:  //Typowe zakoñczenie
       case  27:  //ESC
       case 'q':  //Zakoñczenie zdefiniowane przez programiste
       case 'Q': not_finished=false;break;             
       default:
            print(0,screen_height()-char_height('N'),"Nie wiem co znaczy %c [%d] ",pom,pom);
            printf("Nie wiem co znaczy %c [%d] ",pom,pom);
            flush_plot();	// Grafika gotowa   
            break;
        }
      }
      else //Symulacja - jako akcja na wypadek braku zdarzeñ do obslugi
      {
       single_step(); //Nastêpny krok
       if(step_counter%VISUAL==0) //Odrysuj gdy reszta z dzielenia równa 0
       {
             replot();
             stats();//UWAGA! Nie  stat()!
       }
       if(loop%3==0)
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

void read_mouse()
//Procedura obslugi myszy
{ 
   int xpos,ypos,click;
   if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
   {
      xmouse=xpos;ymouse=ypos;
      //TODO - zaimplementowaæ jeœli potrzebne                                   
      //...
   }                      
}

void write_to_file()
{
     const char* NazwaPliku=NAZWAMODELU ".out";//U¿ywamy sztuczki ze zlepianiem sta³ych 
                                        //³añcuchowych przez kompilator
     std::ofstream out(NazwaPliku); //Nazwa na razie ustalona z góry
     //TODO - funkcja powinna zapisaæ wyniki modelu do pliku zamiast wyrysowaywaæ na ekranie
     //Format - tabela liczb odpowieniego typu rozdzielanych tabulacjami
     //out<<"L i c z b y:\n"<<a[]<<'\t'<<std::endl;
     
     out.close();
}

void screen_to_file()
//Zapis ekranu do pliku (tylko Windows!)
{
   char bufor[255];//Tymczasowe miejsce na utworzenie nazwy pliku
   sprintf(bufor,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
   dump_screen(bufor);
}
