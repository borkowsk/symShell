////////////////////////////////////////////////////////////////////////////////
//		     	Szablon automatu komórkowego np. do regu³y LIFE CONWAYA
//                   (przykladowy program SYMSHELL'a) 
//------------------------------------------------------------------------------
// Prosta obs³uga grafiki, ale z odtwarzaniem ekranu i obslug¹ zdarzen.
// POD Dev-Cpp potrzebne s¹ dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////

#include "symshell.h"
#include <cstdio>    //Wyjœcie na konsole a la jêzyk C - printf(....)
#include <cmath>     //Trochê funkcji matematycznych z jêzyka C
#include <fstream>   // C++ strumienie plikowe
#include <iostream>  // C++ obs³uga konsoli

#define NAZWAMODELU  "Life a la John Conway" //U¿ycie define a nie const char* u³atwia montowanie sta³ych ³añcuchów
//Wyjœciowy rozmiar œwiata i "ekranu" symulacji
const int size=400;

const unsigned RANDOM_SEED=time(NULL); //Zarodek generatora pseudolosowego 
unsigned DENSITY=(size*size)/10;//Musi byæ tyle, ¿eby by³y miejsca z komórkami obok siebie
                        //TODO: manipulacja gêstoœci¹, zw³aszcza dla innych regu³ ni¿ klasyczne
                        
//Klasyczny automat komórkowy zmienia stan wszystkich komórek "jednoczeœnie"
//co w praktyce oznacza, ¿e potrzebna jest jedna tablica na stan aktualny 
//i jedna na przysz³y.
typedef unsigned char WorldType[size][size]; //Zadeklarowanie takiego typu pomaga obejœæ
                                             //pokrêtny sposób deklaracji wskaŸnika do tablicy
//unsigned char ¿eby by³o od 0 do 255, bo typ char bywa te¿ "signed" (zaleznie od kompilatora)
//Dodatkowa zaleta ¿e wystarczy zmieniæ ten typedef i zmieniaj¹ siê wszystkie zmienne 
//tego typu.

WorldType World1;//Pierwsze tablica œwiata - wyzerowana na poczatku bo globalna
WorldType World2;//Druga tablica œwiata - j.w.
WorldType* World=(&World1);//Aktualna tablica œwiata, a w³aœciwie adres do niej

unsigned step_counter=0;//Licznik realnych kroków modelu

void init_world()
//Funkcja do zapoczatkowania œwiata
//Wsypujemy trochê jedynek w losowe miejsca u¿ywaj¹c rand() z C RTL
{
      srand(RANDOM_SEED); //Inicjacja generatora liczb pseudolosowych     
      for(unsigned k=0;k<DENSITY;k++) 
      {
          unsigned i=rand() % size; //% operacja reszta z dzielenia
          unsigned j=rand() % size;//czyli "modulo". Chmmm... *
          World1[i][j]=1; //Gdzieniegdzie coœ ma byæ...        
      }
// * Robienie modulo z wynikiem funkcji rand() jest odradzane przez znawców :-)
//   Ale przy tak ma³ym "swiecie" mo¿e nie zobaczymy problemu      
}

//DEKLARACJA ZAPOWIADAJACA FUNKCJI Regula
//Bêd¹cej w³aœciw¹ implementacj¹ automatu.
unsigned char Regula( unsigned i, //Wiersz komorki
                      unsigned j, //Kolumna komorki
                  //Z jakiego swiata?
                  WorldType* SourceWorld
             );

void single_step()
//Funkcja robi¹ca jeden SYNCHRONICZNY krok symulacji
{
    //Ustalenie co jest stare a co nowe tym razem
    WorldType* OldWorld=NULL;//DLa pewnoœci pusty wskaŸnik 
    unsigned parity=step_counter%2;
    if(parity==0)
    { //Dla wyjœcia z kroku 0 i parzystych
      OldWorld=(&World1);//Œwiat pierwszy jest Ÿród³em danych
      World=(&World2);//Nowym œwiatem (wynikiem) bêdzie drugi œwiat
    }
    else
    { //Dla wyjœcia z kroku 1 i nieparzystych
      OldWorld=(&World2);//Œwiat drugi jest Ÿród³em danych
      World=(&World1);//Nowym œwiatem bedzie ponownie pierwszy œwiat
    }
    //Teraz w³aœciwe wyliczenie nowego stanu automatu
    for(unsigned i=0;i<size;i++)
       for(unsigned j=0;j<size;j++)
       if(Regula(i,j,OldWorld))//UZYCIE FUNKCJI Regula
       {
         (*World)[i][j]=1;//+parity;  
       }
       else
       {
         (*World)[i][j]=0;  
       }
    
    step_counter++;
}

//Fukcja do zamykania œwiata w torus
inline //<-- To funkcja "rozwijana w kodzie" - tylko sk³adnia, bez kosztów wywo³ania
unsigned BezpiecznyOffset(int offset,unsigned start,unsigned WYMIAR)
{
   //Operacje na typie BEZ ZNAKU! G³ówne za³o¿enie - nigdy nie przejœæ poni¿ej zera!!!!!
   unsigned pom=WYMIAR;//Bepieczna "g³ebia"
   pom=pom+start+offset;//Problem by³by tylko gdy ofset ujemny i ABS(offset)>WYMIAR+start
                        //... ale to by by³a du¿a z³oœliwoœæ :-) 
                        //Ale teraz na pewno wyjchodzi poza WYMIAR od strony dodatniej
   pom=pom % WYMIAR;    //To za³atwia "reszta z dzielenia" któr¹ i tak byœmy robili
   return pom;                     
}

//Teraz parametry do uogólnionej regu³a Conwaya. W nawiasach wartoœci zgodne z
//pomys³em oryginalnym, ale mo¿na siê pobawiæ i zmieniæ na coœ innego.
//Jak niezyjesz to siê rodzisz przy conajmniej X(=3) s¹siadach, 
//ale nie wiêcej ni¿ Z(=3)
//A jak ¿yjesz to potrzebujesz nie mniej ni¿ Y(=2) i nie wiêcej niz Z ¿eby prze¿yæ
const unsigned X=3,Y=2,Z=3;//Klasyka
//const unsigned X=1,Y=1,Z=1;//"Migotka"
//const unsigned X=1,Y=1,Z=8;//NIEZNISZCZALNI
//const unsigned X=2,Y=2,Z=3;//Nie lubi¹cy ciasnoty
//const unsigned X=3,Y=2,Z=4;
//const unsigned X=2,Y=1,Z=6;//"LUDZKOSC" :-)
//const unsigned X=4,Y=1,Z=7;//4 p³ciowy - marne szanse ba start
//TODO: Badamy ró¿ne konfiguracje pod zwglêdem zasiedlania przestrzeni 
//TODO2: Dodajemy parametr Z1 ogranicaj¹cy szanse na powstanie m³odego - np.
//mlody powstaje tylko gdy ma 2 rodziców, ale prze¿ywa te¿ gdy ma 3 s¹siadów

//DEFINICJA(implementacja) FUNKCJI Regula
unsigned char Regula( unsigned a,unsigned b, //Parametry mog¹ siê inaczej nazywaæ
             WorldType* SWorld) //ni¿ w deklaracji zapowiadaj¹cej
{
    unsigned suma=0;
    for(int i=-1;i<2;i++)
       for(int j=-1;j<2;j++)
       {  //Zwijanie TORUSA
          unsigned w=BezpiecznyOffset(i,a,size); //w pionie
          unsigned k=BezpiecznyOffset(j,b,size);//i w poziomie
          suma+=(*SWorld)[w][k]==0?0:1; //Zlicza niezerowe     
       }

    if((*SWorld)[a][b]==0)
    {
     return (X<=suma && suma<=Z? 1 : 0 );//Dla "martwych"
    }
    else //Dla ¿ywych
    {
     suma--;//Bo w petli wliczyliœmy srodkowego!!! Tansze ni¿ powtarzany warunek   
     return (Y<=suma && suma<=Z? 1 : 0 );
    }
}



//Statystyki i ich liczenie oraz wyœwietlanie
unsigned alife_counter=0;//Licznik ¿ywych komórek
//TODO3: zapis do pliku...
void stats()
//Funkcja do obliczenia statystyk
{
    alife_counter=0;
    for(int x=0;x<size;x++)
     for(int y=0;y<size;y++)  
      {          //World jest wskaŸnikiem na tablicê, wiêc trzeba go u¿yæ jak 
                 //wskaŸnika, a dopiero potem jak tablicy. Nawias konieczny.
           if((*World)[y][x]!=0)    
             alife_counter++;
      }
    printc(size/2,size,200,64,"%06u  ",alife_counter);//Licznik kroków    
    std::cout<<step_counter<<"      \t"<<alife_counter<<std::endl;
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
      {          //World jest wskaŸnikiem na tablicê, wiêc trzeba go u¿yæ jak 
                 //wskaŸnika, a dopiero potem jak tablicy. Nawias konieczny.
        unsigned z=(*World)[y][x]*200;//Spodziewana wartoœæ to 0 lub 1
        z%=256; //¯eby nie przekroczyæ kolorów
        //z%=512; //Albo z szaroœciami
        plot(x,y,z);
      }
  printc(size/3,size,128,255,"%06u  ",step_counter);//Licznik kroków    
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
 stats();
 flush_plot();	// Ekran lub bitmapa po inicjalizacji jest gotowa 
 screen_to_file();//ODKOMENTOWAÆ jak chcemy materia³ do filmu

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
             stats(); //Uwaga nie "stat" bo to funkcja z RTL jêzyka C!      
             if(loop%3==0)
               printc(0,screen_height()-char_height('C'),128,255,CZEKAM);
             else
               printc(0,screen_height()-char_height('C'),255,128,CZEKAM);
             flush_plot();// gotowe  - i wlasciwie co chwila wywolywane 
             //screen_to_file();//ODKOMENTOWAÆ jak chcemy materia³ do filmu
       }
      
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
     std::cout<<"Zapis stanu do pliku \""<<NazwaPliku<<'"';
     std::ofstream out(NazwaPliku); //Nazwa na razie ustalona z góry
     //TODO - funkcja powinna zapisaæ wyniki modelu do pliku zamiast wyrysowaywaæ na ekranie
     //Format - tabela liczb odpowieniego typu rozdzielanych tabulacjami
     //out<<"L i c z b y:\n"<<a[]<<'\t'<<std::endl;
     for(int x=0;x<size;x++)
     {
      for(int y=0;y<size;y++)  
      {          //World jest wskaŸnikiem na tablicê, wiêc trzeba go u¿yæ jak 
                 //wskaŸnika, a dopiero potem jak tablicy. Nawias konieczny.
        unsigned z=(*World)[y][x];//Spodziewana wartoœæ to 0 lub 1
        out<<z<<'\t';
      }
      out<<'\n';
     }
     out.close();
     std::cout<<std::endl;
}

void screen_to_file()
//Zapis ekranu do pliku (tylko Windows!)
{
   char bufor[255];//Tymczasowe miejsce na utworzenie nazwy pliku
   sprintf(bufor,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
   std::cout<<"Zapis ekranu do pliku \""<<bufor<<'"';
   dump_screen(bufor);
   std::cout<<std::endl;
}
