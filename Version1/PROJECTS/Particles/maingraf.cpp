////////////////////////////////////////////////////////////////////////////////
//		     	Szablon prostego programu symulacyjnego z cz¹steczkami
//                   (przykladowy program SYMSHELL'a) 
//------------------------------------------------------------------------------
// Losowanie przyœpieszeñ jest z rozk³adu "normalnego" w ka¿dym wymiarze
// A potem mo¿na w³¹czyæ zmiany przyspieszeñ :-)
//------------------------------------------------------------------------------
// Prosta obs³uga grafiki, ale z odtwarzaniem ekranu i obslug¹ zdarzen.
// POD Dev-Cpp potrzebne s¹ dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////

#include "symshell.h"
#include <stdio.h> //Wyjœcie na konsole a la jêzyk C - printf(....)
#include <math.h>  //Bêdzie potrzebne trochê matematyki sqrt() itp.
#include <fstream>

#define NAZWAMODELU  "Symulacja cz¹steczkowa" //U¿ycie define u³atwia montowanie sta³ych ³añcuchów
//Wyjœciowy rozmiar œwiata i "ekranu" symulacji
const unsigned size=400;

//Liczba cz¹steczek
const unsigned number=50;

//Typ reprezentuj¹cy cz¹steczkê
struct Particle
{
  double X,Y;//,Z  Po³o¿enie
  double vX,vY;//,vZ Sk³adowe prêdkoœci
  double aX,aY;//,aZ Sk³adowe przyœpieszenia     
};

Particle World[number];//Tablica wszystkich cz¹steczek œwiata        
double TimeStep=0.001; //D³ugoœæ kroku czasu (musi byæ ma³a, mniejsza ni¿ 1
                       //gdy¿ jednostk¹ odleg³oœci jest piksel obrazu)
//WYgodniej mieæ tutaj
const int SRANDV=123; //tylko 0 oznacza losowanie
const int DELA=0;//Jak d³ugie oczekiwanie w obrêbie pêtli zdarzeñ
const int VISUAL=10;//Co ile kroków symulacji odrysowywaæ widok
unsigned step_counter=0;//Licznik realnych kroków modelu

double Rozklad(unsigned ile_iteracji)
//Dzwonowy rozk³ad zmiennej - tym lepszy im wiêcej iteracji
{
  double wynik=0;
  for(unsigned i=0;i<ile_iteracji;i++)
     wynik+=rand()/double(RAND_MAX);//Kolejne decyzje
  wynik/=ile_iteracji;
  return wynik;
}

inline //G³ównie chodzi o strukturê kodu, wiêc nie chcemy p³aciæ za to szybkoœci¹
void init_particle(Particle& P)
{
   //TODO: inicjalizacja z centraln¹ mas¹ i prêdkoœciami "w dysku"
   P.X=size/4+Rozklad(6)*size/2;//Trzeba je jakoœ rozproszyæ
   P.Y=size/4+Rozklad(6)*size/2;//na pocz¹tek
   /*=P.Z=size/2.0;*/ //Z wersji gdzie wszystko startuje z punktu
   P.vX=1.05*(Rozklad(6)-0.5);//Mo¿na te¿ dac jakieœ 
   P.vY=1.05*(Rozklad(6)-0.5);//losowe prêdkoœci, ale ma³e
   /*=P.vZ=0.0;*/ //Poczatkowe prêdkoœci by³y zerowe
   P.aX=0;//za to przyspieszenia na pewno
   P.aY=0;//bêd¹ siê liczyæ same
   //P.aZ=rand()/double(RAND_MAX);
   //printf("A=(%g %g)\n",P.aX,P.aY);//DEBUG
}

void init_world()
//Funkcja do zapoczatkowania œwiata
{
   if(SRANDV)
   srand(SRANDV);//Powtarzanie przebiegu
   else
   srand(time(NULL));//Jeœli nie chcemy powtarzaæ za kazdym razem tego samego
   
   for(unsigned i=0;i<number;i++)
        init_particle(World[i]);
}

inline void change_particle(Particle& P)
//Oddzia³ywania na cz¹stekê, lub jej w³asne decyzje co ma robiæ
{  //By³y losowe zmiany...
   /*P.aX+=0.01*(Rozklad(13)-0.5);
   P.aY+=0.01*(Rozklad(13)-0.5); 
   */
   //Jak zaimplementowaæ przyci¹ganie? UPROSZCZONE
   //Musi byæ obliczone oddzia³ywanie od ka¿dej innej cz¹steczki
   P.aX=P.aY=0;//Liczymy od nowa
   for(unsigned i=0;i<number;i++)//TO JEST KOSZTOWNE!
   if((World+i)!=&P) //Porównanie po adresach bo nie znamy "i" tej cz¹steczki
   {
       double dX=P.X-World[i].X;//X-wa sk³adowa odleg³oœci
       double dY=P.Y-World[i].Y;//Y-wa sk³adowa odleg³oœci
       double Dkw=dX*dX+dY*dY;//Kwadrat odleg³oœci
       if(Dkw>0.0001)//Tylko wtedy jesteœmy w stanie liczyæ. Mo¿e niby byæ 0, ale...
       {
        double D=sqrt(Dkw);
        double a=1;//G*M*m , bo na razie zawsze tak samo
        a/=Dkw; //bo GMm/r2 , wartoœæ przyœpieszenia spada z kwadratem odleg³oœci
        //Ale teraz mamy problem, jak to przyœpieszenie roz³o¿yæ na sk³adowe
        double aX=a*dX/D;//Zmiana przyœpieszenia jakby z proporcji
        double aY=a*dY/D;
        P.aX-=aX;
        P.aY-=aY;
       }
       else
       printf("bec! ");//... a w ogóle to jest zderzenie, z pewn¹ dok³adnoœci¹   
       //TODO: Obs³uga zderzeñ i ró¿nych mas powsta³ych z tego powodu 
   }
}


inline void accelerate_particle(Particle& P)
//Fizyka dzia³ania przyœpieszenia w bardzo ma³ej chwili czasu
{
   P.vX+=TimeStep*P.aX;
   P.vY+=TimeStep*P.aY;
   //P.vZ=TimeStep*P.aZ;
   //printf("V=(%g %g)\n",P.vX,P.vY);//DEBUG
}
 
inline void move_particle(Particle& P)
//Fizyka przesuwania w bardzo ma³ej chwili czasu
{
   P.X+=TimeStep*P.vX;
   P.Y+=TimeStep*P.vY;
   //P.Z+=TimeStep*P.vZ;
   //printf("Poz=(%g %g)\n",P.X,P.Y);//DEBUG
}

void single_step()
//Funkcja robi¹ca jeden krok symulacji - raczej bardzo ma³y krok czasu 
{
    //Coœ mo¿e zmieniaæ przyœpieszenia ( przyci¹gania, odpychania, decyzje! )
    for(unsigned i=0;i<number;i++)
            change_particle(World[i]);
    
    //Przyœpieszenia zmieniaj¹ prêdkoœci
    for(unsigned i=0;i<number;i++)
            accelerate_particle(World[i]);
            
    //Prêdkoœci zmieniaj¹ po³o¿enia
    for(unsigned i=0;i<number;i++)
            move_particle(World[i]);
    
    //I zrobione:
    step_counter++;
}

void stats()
//Funkcja do obliczenia statystyk
{
  //TODO: ... Ile cia³ jest, rozk³ad mas
}


//Do wizualizacji obs³ugi zdarzeñ
const char* CZEKAM="Czekaj i patrz! "; //Monit w pêtli zdarzeñ
int xmouse=10,ymouse=10;//Pozycja ostatniego "kliku" myszy 

//Kilka deklaracja zapowiadaj¹cych inne funkcje obs³uguj¹ce model
void replot(); //Funkcja odrysowyj¹ca
void read_mouse(); // Obsluga myszy - uzywaæ o ile potrzebne
void write_to_file(); // Obsluga zapisu do pliku - u¿ywaæ o ile potrzebne
void screen_to_file(); //Zapis ekranu do pliku (tylko Windows!)

void replot()
//Rysuje coœ na ekranie
{
   //clear_screen();
   //Musimy wiedzieæ jaki mamy aktualnie ekran
   int width=screen_width();
   int heigh=screen_height();
   //Petla po wszystkich cz¹steczkach
   for(unsigned i=0;i<number;i++)
   {
      int x=World[i].X;
      int y=World[i].Y;
      //int z - nie u¿ywamy na razie
      //Czy siê mieœcimy w ekranie?
      if(0<=x && x<=width
      && 0<=y && y<=heigh)
      {
         int c=56+(i*63)%200;//Zeby s¹siednie mia³y inne kolory 
         plot(x,y,c);
      }
   }
  printc(width/2,heigh-char_height('0'),128,255,"%06u  ",step_counter);//Licznik kroków    
}


int main(int argc,const char* argv[])//Potrzebne s¹ paramatry wywo³ania programu
{
 fix_size(0);       // Czy udajemy ¿e ekran ma zawsze taki sam rozmiar?
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
       //case 'c': //TODO: czyszczeni/nie czyszczenie ekranu
       //case 'k': //TODO:centrowanie
       //case 'ijkm'://TODO przesuwanie pola widzenia
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
      
       if(loop%3==0)
        printc(0,screen_height()-char_height('C'),128,255,CZEKAM);
       else
        printc(0,screen_height()-char_height('C'),255,128,CZEKAM);
    
       flush_plot();// gotowe  - i wlasciwie co chwila wywolywane 
       delay_ms(DELA);//Wymuszenie oczekiwania - ¿eby pêtla nie zjada³a ca³ego procesora  
       }
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
     //TODO - funkcja powinna zapisaæ wyniki modelu do pliku zamiast wyrysowywaæ na ekranie
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
