#include <iostream> //Tylko DEBUG
#include <cassert> //Funkcja assert() s³u¿¹ca do sprawdzeñ w fazie produkcji 
#include <cstring> //Bo musimy/chcemy u¿yæ funkcji "memcpy" - kopiowania pamiêci 
#include <cmath> //sin i cos
//#include <stdint.h>
using namespace std;
#include "Wieloboki.hpp"

// Definicja konturów ró¿nych figur
////////////////////////////////////////////
//S¹ "static" tzn. ¿e nie bêd¹ widoczne poza tym modu³em.
//Za to bêd¹ widoczne ich "opakowania" dostêpne z typu Wielobok
static ssh_point kRomb[]= 
{{0,0},{10,10},{0,20},{-10,10}};

static ssh_point kDomek[]=
{{0,0},{10,10},{10,20},{-10,20},{-10,10}};

static ssh_point kUfo[]=
{{0,0},
 {10,10},{20,10},{30,20},{20,30},{10,30},{0,40},
 {-10,30},{-20,30},{-30,20},{-20,10},{-10,10}};
 
static ssh_point kLudzik0[]=
{{0,-12},{1,-12},{2,-11},{2,-10},{1,-8},{4,-6},{4,3},{3,2},{3,-3},{3,0},{2,11},{3,11},{3,12},{0,12},{0,2},
{0,12},{-3,12},{-3,11},{-2,11},{-3,0},{-3,-3},{-3,2},{-4,3},{-4,-6},{-1,-8},{-2,-10},{-2,-11},{-1,-12}};  

static ssh_point kLudzik1[]=
{{0,-12},{1,-12},{2,-11},{2,-10},{1,-8},{4,-6},{4,-2},{3,0},{2,11},{3,11},{3,12},{0,12},{0,2},
{0,12},{-3,12},{-3,11},{-2,11},{-3,0},{-4,-2},{-4,-6},{-1,-8},{-2,-10},{-2,-11},{-1,-12}}; 

//Na razie definijemy przyk³¹dowe kszta³ty zawsze, bo jest ich niewiele
static Wielobok WRomb(kRomb,sizeof(kRomb)/sizeof(kRomb[0]));
static Wielobok WDomek(kDomek,sizeof(kDomek)/sizeof(kDomek[0]));
static Wielobok WUfo(kUfo,sizeof(kUfo)/sizeof(kUfo[0]));
static Wielobok WLudzik0(kLudzik0,sizeof(kLudzik0)/sizeof(kLudzik0[0]));
static Wielobok WLudzik1(kLudzik1,sizeof(kLudzik1)/sizeof(kLudzik1[0]));

//Dostêp Biblioteka podstawowych kszta³tów w przestrzeni nazw klasy Wielobok, 
//Zrobione jako statyczne metody bo to pozwala robiæ potem ró¿ne implementacje 
//przechowywania tych wieloboków, np. tworzyæ je dopiero gdy bêd¹ potrzebne
//lub œci¹gaæ z dysku. 
//TU "static" by³o w klasie i oznacza co innego bo odnosi siê do metody!
const Wielobok&  Wielobok::Romb() {return WRomb;}
const Wielobok&  Wielobok::Domek(){return WDomek;}
const Wielobok&  Wielobok::Ufo() {return WUfo;}
const Wielobok& Wielobok::Ludzik(int typ) 
{
      if(typ==1) return WLudzik1;
         else return WLudzik0;
}  
      

// W£AŒCIWA IMPLEMENTACJA KLASY WIELOBOK
//////////////////////////////////////////////////////////
//Obliczenia istotnych w³aœciwoœci
const int MAXPOS= 2147483647; //INT32_MAX z stdint.h
const int MINPOS=(-2147483647 - 1);//INT32_MIN z stdint.h
int Wielobok::DajMinX() const
{
  int Val=MAXPOS;//Najwiêkszy 
  for(unsigned i=0;i<Ilobok;i++)
  if(Val>Punkty[i].x) //Jeœli Val jest wiêkszy ni¿ X
     Val=Punkty[i].x; //to X zostaje nowym Val 
  return Val; //Najmniejszy X
}

int Wielobok::DajMaxX() const
{
  int Val=MINPOS;//Najmniejszy 
  for(unsigned i=0;i<Ilobok;i++)
  if(Val<Punkty[i].x) //Jeœli Val jest mniejszy od X
     Val=Punkty[i].x; //To X zostaje nowym Val
  return Val; //Najwiêkszy X
}

int Wielobok::DajMinY() const
{
  int Val=MAXPOS;//Najwiêkszy 
  for(unsigned i=0;i<Ilobok;i++)
  if(Val>Punkty[i].y)
     Val=Punkty[i].y;
  return Val; //Najmniejszy Y        
}

int Wielobok::DajMaxY() const
{
  int Val=MINPOS;//Najmniejszy 
  for(unsigned i=0;i<Ilobok;i++)
  if(Val<Punkty[i].y) 
     Val=Punkty[i].y; 
  return Val; //Najwiêkszy Y
}

//Transformacje. Modyfikuj¹ listê punktów ¿eby by³o wygodniej
void Wielobok::Centruj()
//Zmienia wspó³rzêdne tak, ¿eby by³y wokó³ œrodka ciê¿koœci
{
  unsigned i;
  //Najpierw trzeba znaleŸæ œrodek ciê¿koœci figury
  double x=0,y=0;
  for(i=0;i<Ilobok;i++)
  {
     x+=Punkty[i].x;
     y+=Punkty[i].y;        
  }   
  x/=double(Ilobok);
  y/=double(Ilobok);
  
  //A teraz mo¿na przesun¹æ do œrodka
  for(i=0;i<Ilobok;i++)
  {
     Punkty[i].x-=x;
     Punkty[i].y-=y;        
  }         
}

void Wielobok::Skaluj(double x,double y)
//Zmienia wspó³rzêdne
{
    unsigned i;
    for(i=0;i<Ilobok;i++)
    {
     Punkty[i].x*=x;
     Punkty[i].y*=y;        
    }         
}

void Wielobok::OdbijWPionie()
//Zmienia w odbicie lustrzane pionowo
{
    unsigned i;
    for(i=0;i<Ilobok;i++)
    {
     Punkty[i].y=-Punkty[i].y;        
    }        
}

void Wielobok::OdbijWPoziomie()
//Zmienia w odbicie lustrzane poziomo
{
    unsigned i;
    for(i=0;i<Ilobok;i++)
    {
     Punkty[i].x=-Punkty[i].x;     
    } 
}    

void Wielobok::ObrocORad(double Radiany)
//Obraca o ileœ radianów
{
    unsigned i;
    double cosR=cos(Radiany);
    double sinR=sin(Radiany);
    for(i=0;i<Ilobok;i++)
    {
     double x = Punkty[i].x;
     double y = Punkty[i].y;
     Punkty[i].x = x*cosR-y*sinR;       
     Punkty[i].y = x*sinR+y*cosR;
    }
}
     

//Konstruktory - tworz¹ wieloboki na bazie wzorca, tablicy albo innego Wieloboku    
//Nie ma mo¿liwoœci stworzenia pustego "Wieloboku", choæ mo¿e byæ "zerowy"   
//Tylko to ju¿ na odpowiedzialnoœæ u¿ytkownika klasy.  
Wielobok::Wielobok(const ssh_point Wzorek[],unsigned RozmiarWzorka)
{                                                       assert(RozmiarWzorka>2);
   Punkty=new ssh_point[RozmiarWzorka];                 assert(Punkty!=NULL);
   Ilobok=RozmiarWzorka;
   memcpy(Punkty,Wzorek,Ilobok*sizeof(ssh_point));           
}

Wielobok::Wielobok(const Wielobok& Wzorek)
//Konstruktor kopuj¹cy
{
   Punkty=new ssh_point[Wzorek.Ilobok];                  assert(Punkty!=NULL);
   Ilobok=Wzorek.Ilobok;
   memcpy(Punkty,Wzorek.Punkty,Ilobok*sizeof(ssh_point));           
}

//Destruktor - bo trzeba zwolniæ pomocnicz¹ tablice
Wielobok::~Wielobok()
{
  delete []Punkty;
}

//Rysuj() - rysuje gdzieœ wielobok w zadanym kolorze. Nie modyfikuje stanu!
void Wielobok::Rysuj(int StartX,int StartY,ssh_color Color) const
//Rysowanie tej figury w miejscu i kolorze zdefiniowanym gdzie indziej 
//Zrobione jako osobna procedura bo mo¿emy j¹ rozbudowaæ o dodatkowe
//elementy nie bêd¹ce sk³¹dowymi samego wiekok¹ta
{        
   fill_poly(StartX,StartY,Punkty,Ilobok,Color);  
}

const ssh_point&  Wielobok::DajPunkt(unsigned Ktory) const
{
    return Punkty[Ktory];
}

         

