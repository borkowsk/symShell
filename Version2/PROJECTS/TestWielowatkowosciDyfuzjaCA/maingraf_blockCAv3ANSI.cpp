////////////////////////////////////////////////////////////////////////////////
//  Szablon automatu komórkowego zastosowany do uproszczonego pomys³u dyfuzji
//                   (przykladowy program SYMSHELL'a) 
//------------------------------------------------------------------------------
// Prosta obs³uga grafiki, ale z odtwarzaniem ekranu i obslug¹ zdarzen.
// VC++ linkuje biblioteki Windows automatycznie
// POD Dev-Cpp potrzebne s¹ dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////
#define MULTITR (1)  //Jeœli chcemy u¿yæ wielow¹tkowoœci

#include <cstdio>    //Wyjœcie na konsole a la jêzyk C - printf(....)
#include <cmath>     //Trochê funkcji matematycznych z jêzyka C
#include <ctime>
#include <cassert>
#include <fstream>   // C++ strumienie plikowe
#include <iostream>  // C++ obs³uga konsoli
#ifdef MULTITR
#include <thread>    //Obsluga w¹tków
const unsigned max_threads=16;//Maksymalna przewidziana liczba w¹tków
unsigned prefered_threads=3; //Ile w¹tków domyœlnie?
//Ziarno dla w¹tkowego generatora liczb pseudolosowych
__declspec(thread) //thread_local z C++11 powinno dzia³aæ ale w VC++2012 jednak tego nie robi
unsigned long  myholdrand=1;
int myrand()//Na wzór rand() Microsoftu
{
    return( ((myholdrand = myholdrand * 214013L
            + 2531011L) >> 16) & 0x7fff );
}
#endif
#include "SYMSHELL/symshell.h"
#include "INCLUDE/cticker.hpp"
#include "INCLUDE/optParam.hpp"
cticker MyCPUClock;         //Czas od startu programu do liczenia "œredniego czasu kroku brutto"

#define NAZWAMODELU  "2x2 gas v1.0mt " //U¿ycie define a nie const char* u³atwia montowanie sta³ych ³añcuchów
//Wyjœciowy rozmiar œwiata i "ekranu" symulacji
const int MAXSIZE=1000;
int size=500;//Raczej powinno byæ parzyste!

//Do wizualizacji obs³ugi zdarzeñ
const char* CZEKAM="?>"; //Monit w pêtli zdarzeñ
const int DELA=0;//Jak d³ugie oczekiwanie w obrêbie pêtli zdarzeñ
unsigned VISUAL=1;//Co ile kroków symulacji odrysowywaæ widok
int xmouse=10,ymouse=10;//Pozycja ostatniego "kliku" myszy 

unsigned RANDOM_SEED=time(NULL); //Zarodek generatora pseudolosowego 
unsigned DENSITY=(size*size)/100;//Musi byæ tyle, ¿eby by³y miejsca z komórkami obok siebie
                        //TODO: manipulacja gêstoœci¹
                        
wbrtm::OptionalParameterBase* Parameters[]={ //sizeof(Parameters)/sizeof(Parameters[])
new wbrtm::ParameterLabel("PARAMETERS FOR SINGLE SIMULATION"),
new wbrtm::OptionalParameter<unsigned>(DENSITY,1,10000,"DENSITY","How many particles"),
new wbrtm::OptionalParameter<int>(size,0,MAXSIZE,"SIDELEN","Side of the world"),
new wbrtm::ParameterLabel("TECHNICAL PARAMETERS"),
new wbrtm::OptionalParameter<unsigned>(VISUAL,1,10000,"VISUAL","How many steps between visualisation"),
#ifdef MULTITR
new wbrtm::OptionalParameter<unsigned>(prefered_threads,1,max_threads,"THREADS","How many threads"),
#else
new wbrtm::OptionalParameter<unsigned>(RANDOM_SEED,1,0x01FFFFFF,"RANDSEED","Use, if you want the same simulation many times. Do not work with multithreated version"),//Zasiewanie liczby losowej
#endif
new wbrtm::ParameterLabel("END OF LIST")
};    


//Klasyczny automat komórkowy zmienia stan wszystkich komórek "jednoczeœnie"
//co w praktyce oznacza, ¿e potrzebna jest jedna tablica na stan aktualny 
//i jedna na przysz³y.
typedef unsigned char WorldType[MAXSIZE][MAXSIZE]; //Zadeklarowanie takiego typu pomaga obejœæ
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

//Fukcja do zamykania œwiata w torus
inline //<-- To funkcja "rozwijana w kodzie" 
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

unsigned char Rules[16][7]={
{ 0, 0, 0, 0, 0, 0, 0},// --  0000 -> 0x0
					   // --
{ 1, 8, 4, 2, 1, 8, 4},// --  0001 --> 0x1
				       // -x
{ 1, 4, 2, 1, 8, 4, 2},// --  0010 --> 0x2
				       // x-
{ 2,12,10, 9, 6, 5, 3},// --  0011 --> 0x3
				       // xx
{ 1, 2, 1, 8, 4, 2, 1},// -x  0100 --> 0x4
				       // --
{ 2,10, 9, 6, 5, 3,12},// -x  0101 --> 0x5
				       // -x
{ 2, 9, 6, 5, 3,12,10},// -x  0110 --> 0x6
				       // x-
{ 3,14,13,11, 7,14,13},// -x  0111 --> 0x7 
				       // xx
{ 1, 1, 8, 4, 2, 1, 8},// x-  1000 --> 0x8
				       // --
{ 2, 6, 5, 3,12,10, 9},// x-  1001 --> 0x9
				       // -x
{ 2, 5, 3,12,10, 9, 6},// x-  1010 --> 0xa czyli 10
				       // x-
{ 3,13,11, 7,14,13,11},// x-  1011 --> 0xb czyli 11
				       // xx
{ 2, 3,12,10, 9, 6, 5},// x-  1100 --> 0xc czyli 12
				       // x-
{ 3,11, 7,14,13,11, 7},// xx  1101 --> 0xd czyli 13
				       // -x
{ 3, 7,14,13,11, 7,14},// xx  1110 --> 0xe czyli 14
				       // x-
{ 4,15,15,15,15,15,15} // xx  1111 --> 0xf czyli 15
				       // xx
};
//DEFINICJA FUNKCJI RegulaIZmiana
//Bêd¹cej w³aœciw¹ implementacj¹ automatu - tu siê ustala stan aktualny oraz wybiera nastêpny
inline //<-- To funkcja "rozwijana w kodzie"
void RegulaIZmiana(			unsigned i, //Wiersz startowej komorki bloku
							unsigned j, //Kolumna startowej komorki 
							WorldType& SW,//Z jakiego swiata?
							WorldType& TW //Do jakiego œwiata
							//,unsigned& rstate //stan generatora rand_r
             )
{
	unsigned le=(j+1)%size; //Zamkniêcie w torus
	unsigned bo=(i+1)%size; //jest uproszczone...
	unsigned char old=(SW[i][ j]&1)*8 + (SW[bo][ j]&1)*4
					 +(SW[i][le]&1)*2 + (SW[bo][le]&1)*1;
															assert(old<16);		
	//unsigned randVal=4;       //Test bez rand()
	//unsigned randVal=rand_r(&rstate);//Niestety ju¿ "obsolete" i nie ma jej w stdlib
#ifdef MULTITR
	unsigned randVal=myrand(); //u¿ywa w¹tkowo specyficznego stanu 
#else
	unsigned randVal=rand();	//Czy rand() jest jakoœ zabezpieczone wzglêdem w¹tków? OGÓLNIE W¥TPIE. W  MSVC++ nie
#endif

    unsigned char nex=old==0?0:old==15?15:Rules[old][1+randVal%6];//Sprawdzenie stanu, ale 0 i 15 nie maj¹ szans na zmianê
															assert(nex<16);
															assert(Rules[old][0] == Rules[nex][0]);
	TW[ i][ j]= nex&8?1:0; 
	TW[bo][ j]= nex&4?1:0;
	TW[ i][le]= nex&2?1:0; 
	TW[bo][le]= nex&1?1:0;
}

#ifdef MULTITR
void doMove(int StartLine,int EndLine,int Parity, WorldType* pSW,WorldType* pTW)
{
	 //printf("%u %u\n",StartLine,EndLine);fflush(stdout);
	 myholdrand=time(NULL)+int(&EndLine);//Jakieœ Ÿró¿nicowane Ÿród³o ziarna
	 for(unsigned i=StartLine;i<=EndLine;i+=2)
	 {                                                              assert(i<size);
	   for(unsigned j=Parity;j<size;j+=2)      
		RegulaIZmiana(i,j,*pSW,*pTW);
	 }
}
#endif

void single_step()
//Funkcja robi¹ca jeden SYNCHRONICZNY krok symulacji
{
    //Ustalenie co jest stare a co nowe tym razem
    WorldType* OldWorld=NULL;//DLa pewnoœci pusty wskaŸnik 
    unsigned parity=step_counter%2;
	if(parity==0)//Dla wyjœcia z kroku 0 i parzystych
    { 
      OldWorld=(&World1);World=(&World2);//Œwiat pierwszy jest Ÿród³em danych a nowym œwiatem (wynikiem) bêdzie drugi œwiat
    }
    else //Dla wyjœcia z kroku 1 i nieparzystych
    { 
      OldWorld=(&World2); World=(&World1);//Œwiat drugi jest Ÿród³em danych a nowym œwiatem bedzie ponownie pierwszy œwiat
    }

//Teraz w³aœciwe wyliczenie nowego stanu automatu
#ifdef MULTITR
	std::thread theThreads[max_threads];  
	int lines_per_thr=(size/2)/prefered_threads;//Tylko co druga linia mo¿e byæ startow¹
	int rest_lines=(size/2)%prefered_threads;//W¹tek pierwszy dostaje ca³¹ resztê
	int fl=parity;
	int ll=fl+(rest_lines+lines_per_thr)*2-1;//ll to indeks ostatniej lini  
	for(int t=0;t<prefered_threads;t++)
	{
		theThreads[t]=std::thread(doMove,fl,ll,parity,OldWorld,World);//T1: doMove(parity,size/pref-1,parity,OldWorld,World);
		fl=ll+1;ll=fl+lines_per_thr*2-1;
		//if(ll>=size) ll=size-1;//To ostatnie mo¿liwa linia 
	}
	//G³ówny bêdzie sobie czeka³
	for(int t=0;t<prefered_threads;t++) 
		theThreads[t].join();
#else
 	for(unsigned i=parity;i<size;i+=2)
       for(unsigned j=parity;j<size;j+=2)
			RegulaIZmiana(i,j,*OldWorld,*World);
#endif
    step_counter++;
}

//Statystyki i ich liczenie oraz wyœwietlanie
unsigned alife_counter=0;//Licznik ¿ywych komórek - do u¿ycie te¿ w wyœwietlaniu wiêc globalny

#ifdef MULTITR
void doStat(int StartLine,int EndLine,unsigned& Summ)
{
	//printf("%u %u\n",StartLine,EndLine);fflush(stdout);
	unsigned tmp=0;
	for(int y=StartLine;y<=EndLine;y++)//Tylko wskazana czêœæ œwiata
	  for(int x=0;x<size;x++)//World jest wskaŸnikiem na tablicê, wiêc trzeba go u¿yæ jak 
           if((*World)[y][x]!=0)     //wskaŸnika, a dopiero potem jak tablicy. Nawias konieczny.
             tmp++;
	Summ=tmp;
}
#endif

void statistics()
//Funkcja do obliczenia statystyk
{
	 alife_counter=0;
#ifdef MULTITR
	//const unsigned max_threads=16;//Maksymalna przewidziana liczba w¹tków
	//unsigned prefered_threads=3; //Ile w¹tków domyœlnie?
	unsigned part_alife_counters[max_threads];
	std::thread* theThreads[max_threads];  
	int lines_per_thr=size/prefered_threads;
	int rest_lines=size%prefered_threads;
	for(int fl=0,ll=lines_per_thr-1,t=0;t<prefered_threads;t++)//ll to indeks ostatniej lini  
	{
		part_alife_counters[t]=0;//
		if( rest_lines-- > 0) 
			ll++; //Puki jest reszta z dzielenia, rozk³adamy j¹ na w¹tki które wystartuj¹ pierwsze
		theThreads[t]=new std::thread(doStat,fl,ll,std::ref(part_alife_counters[t]));
		fl=ll+1;ll+=lines_per_thr;
	}
	//G³ówny bêdzie sobie czeka³
	for(int t=0;t<prefered_threads;t++)
	{
		theThreads[t]->join();
		alife_counter+=part_alife_counters[t];//Musi byæ po join!
		delete theThreads[t];
	}
#else
    for(int x=0;x<size;x++)
     for(int y=0;y<size;y++)  
      {          //World jest wskaŸnikiem na tablicê, wiêc trzeba go u¿yæ jak 
                 //wskaŸnika, a dopiero potem jak tablicy. Nawias konieczny.
           if((*World)[y][x]!=0)    
             alife_counter++;
      }
#endif
    printc(size/2,size,200,64,"%06u  ",alife_counter);//Licznik kroków    
   // std::cout<<step_counter<<"      \t"<<alife_counter<<std::endl;
}

//TODO 3: zapis do pliku...

//Kilka deklaracja zapowiadaj¹cych inne funkcje obs³uguj¹ce model
void replot(); //Funkcja odrysowyj¹ca
void read_mouse(); // Obsluga myszy - uzywaæ o ile potrzebne
void write_to_file(); // Obsluga zapisu do pliku - u¿ywaæ o ile potrzebne
void screen_to_file(); //Zapis ekranu do pliku (tylko Windows!)

void replot()
//Rysuje coœ na ekranie
{
  for(int x=0;x<size-1;x++)
    for(int y=0;y<size-1;y++)  
      {          //World jest wskaŸnikiem na tablicê, wiêc trzeba go u¿yæ jak 
                 //wskaŸnika, a dopiero potem jak tablicy. Nawias konieczny.
        unsigned z=(*World)[y][x]*200;//Spodziewana wartoœæ to 0 lub 1
        z%=256; //¯eby nie przekroczyæ kolorów
        //z%=512; //Albo z szaroœciami
        plot(x,y,z);
      }
  printc(size/5,size,128,255,"%06u MstT:%g  ",step_counter,(double)MyCPUClock/step_counter);//Licznik kroków    
  //Ostatnie polozenie kliku - bia³y krzy¿yk   
  //line(xmouse,ymouse-10,xmouse,ymouse+10,255);
  //line(xmouse-10,ymouse,xmouse+10,ymouse,255);
}


int main(int argc,const char* argv[])//Potrzebne s¹ paramatry wywo³ania programu
{
 printf("Model \"%s\". File version %s\n",NAZWAMODELU,__TIMESTAMP__);
 if(wbrtm::OptionalParameterBase::parse_options(argc,argv,Parameters,sizeof(Parameters)/sizeof(Parameters[0])))
 {
			exit(222);
 }
#ifdef MULTITR
 printf("Number of hardware concurent contexts: %d\n",std::thread::hardware_concurrency());
 printf("Default number of threads: %d\n\n",prefered_threads); //Ile w¹tków domyœlnie?
 myholdrand=2;     //INICJACJA myrand() dla g³ównego w¹tku - pewnie nieistotna
#endif
 fix_size(1);       // Czy udajemy ¿e ekran ma zawsze taki sam rozmiar?
 mouse_activity(0); // Czy mysz bêdzie obs³ugiwana?   
 buffering_setup(1);// Czy bêdzie pamietaæ w bitmapie zawartosc ekranu? 
 shell_setup(NAZWAMODELU,argc,argv);// Przygotowanie okna z u¿yciem parametrów wywo³ania
 init_plot(size,size,0,1);/* Otwarcie okna SIZExSIZE pixeli + 1 wiersz znakow za pikselami*/

 // Teraz mo¿na rysowaæ i pisac w oknie 
 init_world();  //Tu jest te¿ srand();
 replot();
 statistics();
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
             statistics(); //Uwaga nie "stat" bo to funkcja z RTL jêzyka C!      
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
#ifdef _MSC_VER /*MSVC*/
   _snprintf_s(bufor,255,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
#else
   sprintf(bufor,"%s%06u",NAZWAMODELU,step_counter);//Nazwa + Numer kroku na 6 polach
#endif
   std::cout<<"Zapis ekranu do pliku \""<<bufor<<'"';
   dump_screen(bufor);
   std::cout<<std::endl;
}
