// WIELOBOKI W SIECI EPIDEMII - program SYMSHELLA pokazuj¹cy idea agenta-obiektu
////////////////////////////////////////////////////////////////////////////////
// POD Dev-Cpp potrzebne s¹ dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" 
// oraz "...Dev-Cpp/lib/libcomdlg32.a"

#include "symshell.h"
#include "Wieloboki.hpp"
#include <iostream>
#include <fstream> //Bo plik statystyki
#include <cstdlib>
#include <cmath>
#include <cassert> //DO WY£APYWANIA KODU DZIA£AJ¥CEGO NIEZGODNIE Z ZA£O¯ENIAMI
using namespace std;

const int size=500; //Bok obszaru roboczego
const int iluich=40; //ILU JEST AGENTÓW!
const int ilechoroby=7; //Katar leczony trwa 7 dni a nieleczony tydzieñ
const int ileodpornosci=60; //Potem ju¿ inny szczep wirusa

//Agent epidemiczny musi mieæ pozycje, stan: zdrowy, od kiedy chory, od kiedy odporny
//Mo¿e mieæ te¿ aktualny kolor, zmieniaj¹cy siê wraz ze zmian¹ stanów
//Musi te¿ mieæ informacje o po³¹czeniach z innymi agentami bo to model sieciowy
class Agent
////////////////////////////////////////////////////////////////////////////////
{
  Wielobok  Ksztalt;
  double Xpoz;
  double Ypoz;
  ssh_color Kolor;
  unsigned Stan;//jak 0 to zdrowy, ka¿da inna wartoœæ to dni od zara¿enia
  protected:
  //Z kim agent mo¿e wchodziæ w interakcje?
  int Powiazania[iluich];//Jednowymiarowa tablica - byæ mo¿e nadmiarowa, ale mamy du¿o pamiêci
  //Trzeba jakoœ ustaliæ wagi po³¹czeñ
  friend void generuj_siec_powiazan(); //Musi miec dostêp do tablicy po³¹czeñ agenta
  public:
  //Konstruktor  bezparametrowy - konieczny jeœli maj¹ byæ w tablicy     
  Agent():Ksztalt( Wielobok::Ludzik(0) )//,Xpoz(0),Ypoz(0) - mog³o by byæ
     {
        Xpoz=rand()%size;
        Ypoz=rand()%size;
        Ksztalt.Centruj();//¯eby œrodek by³ w œrodku :-)
        Ksztalt.Skaluj(2,2);//I wielkoœæ jakaœ rozs¹dna
        //Tymczasowy kolor - NIECO ZMIENIONY ZE WZGLÊDU NA KOLOROWANIE EPIDEMII
        Kolor=55+rand()%145;//Bo pierwsze 50 kolorów jest ciemne, a powy¿ej 200 zarezerowane 
        //Wstêpny stan
        Stan=0;
        //Wstêpne ustawienie powi¹zañ obojetne czyli brak     
        for(unsigned i=0;i<iluich;i++) Powiazania[i]=0; //Zadzia³a te¿ gdy float!
     }
  //Destruktor - na razie nic jawnie nie robi
  virtual ~Agent(){} //ale na wszelki wypadek wirtualny 
  //- destrukcja rzadko, wiêc to w³asciwie nic nie kosztuje jeœli chodzi o czas CPU
   
  //Czytanie atrybutów. "const" dla zaznaczenia ¿e tylko mo¿emy czytaæ
  double X() const {return Xpoz;}
  double Y() const {return Ypoz;}   
  unsigned St() const {return Stan;}
  //Ustawianie atrybutów
  void  SetXY(double X,double Y); //Po³o¿enie
  void  Zaraza();   //Zara¿anie - zawsze z 0 na 1, inne ignoruje        
  //Do symulacji musi miec dostêp do tablicy agentów    
  void ZrobKrok(Agent Swiat[],unsigned ilu); 
  //Do wizualizacji        
  void Rysuj() //Rysowanie agenta
     {Ksztalt(Xpoz,Ypoz,Kolor);}    
  //Rysowanie wszystkich agentów i po³¹czeñ miêdzy nimi
  friend void replot(); //ZaprzyjaŸniona bo musi mieæ dostêp do powi¹zañ 

};

//G³ówna dynamika modelu jest w dzia³aniach agentów
////////////////////////////////////////////////////////////////////////////////
void Agent::Zaraza()
//Zara¿anie - zawsze z 0 na 1, inne ignoruje        
{
   if(Stan==0 //Mo¿e siê zaraziæ jak nigdy siê nie zetkn¹³
   || Stan>ilechoroby//=7; //Katar leczony trwa 7 dni a nieleczony tydzieñ
      +ileodpornosci//Potem ju¿ inny szczep wirusa   //Albo jak straci³ odpornoœæ         
      )
   {
       Stan=1;//Zara¿ony, byæ mo¿e ponownie
       //Zmiana koloru
       Kolor=255+30; //Pow 255 odcienie szaroœci w symshell'u 
   }      
}

void Agent::ZrobKrok(Agent Swiat[],unsigned ilu)
{
    //Postêp ewentualnej choroby lub odpornoœci (liczba dni od zara¿enia)
    if(Stan>0) 
    {
     Stan++;
    //Zmiana koloru - jeœli zara¿ony lub odporny. Uwaga! Czas od zara¿enia mo¿e byæ d³ugi
     if(Stan<=ilechoroby) 
     {
           Kolor=255+30+Stan*10;//Czy nie przekracza 512?
           //if(Kolor>=512) cerr<<"KolorI? "<<Kolor<<" Stan="<<Stan<<endl;//DEBUG
     }
     else
     if(Stan<=ileodpornosci) 
     {
           Kolor=200+(54*(ileodpornosci-Stan)/ileodpornosci);//od 255 do 200
           //if(Kolor>=512) cerr<<"KolorII? "<<Kolor<<" Stan="<<Stan<<endl;//DEBUG
     }
     else //Straci³ odpornoœæ, miga sobie ¿eby by³o widaæ ¿e podatny ponownie
     {
          Kolor=55+(rand()%145);//Bo pierwsze 50 kolorów jest ciemne, a powy¿ej 200 zarezerowane 
          //if(Kolor>=512) cerr<<"KolorIII? "<<Kolor<<" Stan="<<Stan<<endl;;//DEBUG
     }
                                                              assert(Kolor<512);
    }
    //Wchodzi w losowe interakcje z pod³¹czonymi agentami  
    //i mo¿e ich zaraziæ. ALE KTOŒ MUSI BYÆ NA POCZ¥TKU (w inicjalizuj()) ZARA¯ONY
    //Wersja najprostrza: ka¿dy raz ma szanse na interakcje i zara¿a jak jest chory
    if(0<Stan && Stan<=ilechoroby)
    {
     unsigned partner=rand()%iluich; //Jakiœ z listy
                                                         assert(partner<iluich);
     if(Powiazania[partner]!=0)
          Swiat[partner].Zaraza();
    }
}

Agent SwiatAgentow[iluich];//"KOLEKCJA" AGENTÓW - NIE BÊD¥ ZNIKAÆ TO MO¯E BYÆ TABLICA 

//Reszta programu s³u¿y temu, ¿eby te dzia³ania mioa³y sens
////////////////////////////////////////////////////////////////////////////////
void  Agent::SetXY(double X,double Y)
//Po³o¿enie
{
   if(0<X && X<size) //Gdy mieœci siê na ekranie
             Xpoz=X; //to zapisujemy na atrybucie
   if(0<Y && Y<size) //Gdy mieœci siê na ekranie
             Ypoz=Y; //Tak samo dla Y
}

void generuj_siec_powiazan()
//Mozna zrobiæ siec:
// * losow¹ o jakiejœ gêstoœci
// * sieæ losow¹ o jakimœ rozk³adzie wag
// * sieæ regularn¹ o zadanej odleg³oœci powi¹zañ
// * sieæ ma³ych œwiatów (regularn¹ + dodatkowe odleg³e po³¹czenia)
// * itd, itp, etc ... 
{
     //Sieæ losowa o zadanym udziale powiazañ
     unsigned N=iluich;//Ile jest agentów
     unsigned M=((N*N)-N)/2; //Ile maksymalnie powi¹zañ symetrycznych
     M=M*0.15;//jakiœ u³amek mo¿liwych - nie za du¿o bo nic nie zobaczymy
                      //A w czasie epidemi wszyscy zachoruj¹
     unsigned licznik=0;//zliczanie udanych po³¹czeñ
     do
     {
       unsigned pierwszy=rand()%iluich;
       unsigned drugi=rand()%iluich;
       cout<<pierwszy<<"->"<<drugi;//DEBUG
       
       if(pierwszy!=drugi
       && SwiatAgentow[pierwszy].Powiazania[drugi]==0 //Wystarczy sprawdziæ w jednym
       )
       {
         cout<<" W="<<1;//DEBUG
         //Symetrycznie. Na razie ka¿da dana zdublowana
         //ale nie trzeba bêdzie du¿o zmieniaæ jak uznamy 
         //¿e po³¹czenia s¹ jednak asymetryczne
         SwiatAgentow[pierwszy].Powiazania[drugi]=1;
         SwiatAgentow[drugi].Powiazania[pierwszy]=1;
         licznik++; //udany
       }
       cout<<endl; //DEBUG
     }while(licznik<M);
     //inne rodzaje: TODO, TODO, TODO...
}

void inicjalizacja()
//U³o¿enie pocz¹tkowe i inne operacje konieczne do zrobienia na pocz¹tku
{
   srand(time(NULL)); //Inicjalizacja generatora       

  //Reinicjalizacja pozycji agentów - na to czego nie by³o w konstruktorze  
   for(unsigned i=0;i<iluich;i++) 
   {
     //Pozycje w kó³ku, jak to czêsto siê stosuje przy sieciach spo³ecznych
      double alfa=((2*M_PI)/iluich)*i; 
      double x=size/2.0+cos(alfa)*size/2.34;//Promieñ mniejszy 
      double y=size/2.0+sin(alfa)*size/2.34;//... ¿eby im g³ówki nie wychodzi³y 
      SwiatAgentow[i].SetXY(x,y);
      cout<<x<<' '<<y<<endl;//DEBUG
   } 
   
   //Jakiœ rodzaj sieci - moze byæ zalezny od pozycji wieæ tu
   generuj_siec_powiazan();
   
   //PIERWSZY START EPIDEMII
   //Inicjalne zara¿anie jakiegoœ agenta
   ///////////////////////////////////////////////////
   SwiatAgentow[0].Zaraza();
}     

void krok()
//Zmiany u³o¿enia zwi¹zane z ró¿nymi ruchami
{       
   //Ka¿dy agent mo¿e coœ zrobiæ - efekty widoczne potem
   for(unsigned i=0;i<iluich;i++)
    SwiatAgentow[i].ZrobKrok(SwiatAgentow,iluich);          
   
   //Inny sposób zapocz¹tkowywania epidemii 
   if(rand()/double(RAND_MAX)<0.05) //Raz na 100 kroków srednio
   {
     unsigned ktory=rand()%iluich;
     SwiatAgentow[ktory].Zaraza();
     cout<<"Nowy start epidemii u agenta "<<ktory<<endl;
   }
                                                
}

void replot()
//Procedura odrysowujaca aktualny stan
{
  clear_screen();//Czyszczenie ca³ego ekranu na wszelki wypadek
  circle(size/2,size/2,10,128);
  //W wizualizacja sieci
  unsigned x1,y1,x2,y2;
  for(unsigned i=0;i<iluich;i++)//Ka¿dy agent
  {
   x1=SwiatAgentow[i].X();
   y1=SwiatAgentow[i].Y();
   for(unsigned j=0;j<iluich;j++)//ma jakiœ powi¹zanych
    if(SwiatAgentow[i].Powiazania[j]!=0)
    { //... linie miêdzy agentami - na razie nieoptymalnie, 
      //bo ka¿da linia dwa razy. TODO: Jak zrobiæ ¿eby raz tylko
     x2=SwiatAgentow[j].X();
     y2=SwiatAgentow[j].Y();
     unsigned char Kol=50+rand()%200;
     line(x1,y1,x2,y2,Kol);
    }
  }
  //Wizualizacja agentów
  for(unsigned i=0;i<iluich;i++)
    SwiatAgentow[i].Rysuj();
   
  flush_plot();	// Bitmapa gotowa 
}

ofstream OutStat;

void statystyka() //TODO - brak numeracji kroków czasu
{ //ilu bez kontaktu, ilu chorych, ilu odpornych, ilu ponownie podatnych
  unsigned bezKontaktu=0;
  unsigned chorych=0;
  unsigned odpornych=0;
  unsigned podatnych=0;
  //Zliczanie agentów z ró¿nymi stanami
  for(unsigned i=0;i<iluich;i++)
  {
    unsigned Stan=SwiatAgentow[i].St();
    if(Stan==0) bezKontaktu++;
    else
    if(Stan<=ilechoroby) chorych++;
    else
    if(Stan<=ilechoroby+ileodpornosci) odpornych++;
    else
    podatnych++; 
  }
  //Wypis stanu w specjalnej linii na tekst (koñcowej) ekranu kraficznego
  printc(size/2,size,50,255,"S:%u I:%u R:%u rS:%u    ",bezKontaktu,chorych,odpornych,podatnych);
  if(!OutStat.is_open())//Jak nie jest otwarte to wiemy ¿e trzeba otworzyæ
  {
     OutStat.open("epidemia.out",ios_base::app); //Ka¿d¹ epidemie dodaje do pliku z wynikami
     time_t      szClock; //Wg. przyk³adu 
     struct tm   *newTime;//z manuala MVC++
     time( &szClock ); //Czas w sekundach "ery komputerowej"
     //cyt.:"Return the time as seconds elapsed since midnight, January 1, 1970, or -1 in the case of an error."
     newTime = localtime( &szClock ); //Przekonwertowanie na strukturê
    //Uzycie do wydrukowania
     OutStat<<asctime(newTime ); //Linia daty i czasu ma w³asne \n !!!
     //Linia nag³ówków
     OutStat<<"Susp.\tInfr.\tReco.\treSu"<<endl;
  }
  //Zwyczajnie kolejne dane
  OutStat<<bezKontaktu<<"\t"<<chorych<<"\t"<<odpornych<<"\t"<<podatnych<<endl; 
  OutStat.flush(); //Bufor na dysk
}

//Zmienne do obs³ugi zdarzeñ i dalej funkcje które to robi¹, w tym main()
const int DELA=100;//Oczekiwanie w obrêbie pêtli
const int WAIT=10; //Co ile kroków migac napis
const char* CZEKAM="Patrzaj ino..."; 
unsigned long loop=0;

void read_mouse() //Procedura obslugi "kliku" myszy, np. do inspekcji agenta
{ 
   int xpos,ypos,click;
   if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
   {
      //.....x=xpos;
      //.....y=ypos;
      //...TODO...Zaznacz innym kolorem wszystkie po³¹czenia agenta
      replot();   
      //...TODO i czekaj na jakiœ klik, ¿eby by³ czas zobaczyæ                                                
   }                      
}

int main(int argc,char* argv[])//Potrzebne s¹ paramatry wywo³ania programu
{
   mouse_activity(1);       //Czy mysz bêdzie obs³ugiwana? TAK, ale potem
   buffering_setup(1);      //Czy pamietac w bitmapie zawartosc ekranu? NIE lub TAK
   fix_size(1);	            //Czy symulowaæ niezmiennosci rozmiarow okna? TAK
   shell_setup("PRAWIE AGENTOWA SYMULACJA EPIDEMII",argc,argv);//Ustalenie tytu³u i konfiguracji z linii komend
   init_plot(size,size,0,1);// size x size pixeli + 1 wiersz znakow pod spodem
   
   inicjalizacja();         //Przygotowanie paramerów
   replot();  //Rysowanie 
   statystyka(); //Obliczanie chwilowych statystyk
   
    bool kontynuuj=true;	//Zmienna steruj¹ca zakoñczeniem programu
    while(kontynuuj)        //PÊTLA OBS£UGI ZDARZEÑ
    { 
      int pom;
      loop++; //Zliczanie nawrotów
      if(input_ready()) //Czy jest coœ do obs³ugi?
      {
       pom=get_char(); //Przeczytaj nades³any znak
       switch(pom)
       {
       case '\r': replot();break;//Wymagane odrysowanie
       case '\b': read_mouse();break;//Zdarzenie myszy
       case EOF:  //Typowe zakoñczenie
       case  27:  //ESC
       case 'q':  //Zakoñczenie zdefiniowane przez programiste
       case 'Q': kontynuuj=false;break;             
       default:
            print(0,screen_height()-char_height('N'),"Nie wiem co znaczy %c [%d] ",pom,pom);
            flush_plot();	// Bitmapa gotowa   
            break;
        }
     cout<<"Zareagowano na kod nr "<<int(pom)<<" w nawrocie "<<loop<<endl;   
  }
  else //Akcja na wypadek braku zdarzeñ
  {
  krok();    //Poruszanie
  replot();  //Rysowanie  
  statystyka(); //Obliczanie chwilowych statystyk
 
  if(loop%WAIT==0)
     printc(0,screen_height()-char_height('C'),128,255,"%s %d  ",CZEKAM,loop);
  else
     printc(0,screen_height()-char_height('C'),255,28,"%s %d  ",CZEKAM,loop);       
  flush_plot();// Bitmapa po modyfikacji ju¿ gotowa   
  delay_ms(DELA);//Wymuszenie oczekiwania - ¿eby pêtla nie zjada³a ca³ego procesora  
             //I ¿eby siê wszystko nie dzia³o zbyt szybko!
  } 
}

cout<<"Wykonano "<<loop<<" obrotow petli.\nDo widzenia!\n"<<endl;
close_plot(); //Po zabawie - zamykam okno
return 0;
}
