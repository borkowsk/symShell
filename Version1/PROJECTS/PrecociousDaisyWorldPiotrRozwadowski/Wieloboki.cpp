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
static ssh_point kSurface[]= 
{
     {5,0},{15,0},{20,9},{15,18},{5,18},{0,9}
};

static ssh_point kCenter_plant[]=
{
     {9,7},{10,7},{11,8},{11,9},{10,10},{9,10},{8,9},{8,8}
};

static ssh_point kPlant[]=
{
     {5,1},{6,1},{7,2},{8,2},{8,3},{10,5},{11,4},{11,2},{13,2},{13,1},{14,1},
     {14,4},{11,7},{12,8},{13,7},{14,7},{15,6},{16,6},{18,8},{18,9},{16,11},
     {15,11},{14,10},{13,10},{12,9},{11,10},{14,13},{14,16},{13,16},{12,15},
     {11,15},{11,14},{9,12},{8,13},{8,15},{7,15},{6,16},{5,16},{5,13},{8,10},
     {7,9},{6,10},{5,10},{4,11},{3,11},{1,10},{1,8},{3,6},{4,6},{5,7},{6,7},
     {7,8},{8,7},{5,4} 
};
 
static ssh_point kSun[]= 
{
       {84,0},{92,45},{117,31},{107,54},{144,44},{115,68},
       {137,76},{116,83},{149,107},{111,101},{121,119},{102,111},
       {107,150},{86,118},{74,137},{69,118},{41,145},{51,111},
       {29,116},{40,100},{0,105},{37,80},{17,71},{37,65},{12,37},
       {50,49},{51,24},{70,43}
       }; 

static ssh_point kSun_center[]= 
{
       {40,1},{54,5},{67,12},{75,25},{77,36},{72,54},{65,62},{53,69},{39,71},
       {25,69},{14,62},{6,53},{2,37},{6,20},{14,11},{25,5}
       
       }; 

static ssh_point kLudzik2[]= //Pierwsza kobieta
{{0,-12},
{1,-12},{2,-11}, {3,-9},{2,-10},{1,-9},{1,-8},{3,-7},{4,-5},{4,0},{3,1},{3,-4},{2,-2},{3,1},{1,11},{2,11},{2,12},{0,12},
{0,2},
{0,12}, {-2,12},{-2,11},{-1,11},{-3,1},{-2,-2},{-3,-4},{-3,1},{-4,0},{-4,-5},{-3,-7},{-1,-8},{-1,-9},{-2,-10},{-3,-9},{-2,-11},{-1,-12},
}; 

static ssh_point kLudzik3[]= //Facet 2
{{0,-12},{1,-12},{2,-11},{2,-10},{1,-8},{4,-6},{4,3},{3,2},{3,-3},{3,0},{2,11},{3,11},{3,12},{1,12},{0,2},
{-1,12},{-3,12},{-3,11},{-2,11},{-3,0}, {-3,-3},{-3,2}, {-4,3}, {-4,-6}, {-1,-8}, {-2,-10},{-2,-11},{-1,-12}};  

static ssh_point kLudzik4[]= //druga kobieta
{{0,-12},
{1,-12},{2,-11}, {3,-9},{3,-10},{1,-9},{1,-8},{3,-7},{4,-5},{5,-1},{4,1},{3,-4},{2,-2},{3,1},{1,11},{2,11},{2,12},{0,12},
{0,2},
{0,12}, {-2,12},{-2,11},{-1,11},{-3,1},{-2,-2},{-3,-4},{-4,1},{-5,-1},{-4,-5},{-3,-7},{-1,-8},{-1,-9},{-3,-10},{-3,-9},{-2,-11},{-1,-12},
}; 

//Na razie definijemy przyk³¹dowe kszta³ty zawsze, bo jest ich niewiele
static Wielobok WSurface(kSurface,sizeof(kSurface)/sizeof(kSurface[0]));
static Wielobok WCenter_plant(kCenter_plant,sizeof(kCenter_plant)/sizeof(kCenter_plant[0]));
static Wielobok WPlant(kPlant,sizeof(kPlant)/sizeof(kPlant[0]));
static Wielobok WSun(kSun,sizeof(kSun)/sizeof(kSun[0]));
static Wielobok WSun_center(kSun_center,sizeof(kSun_center)/sizeof(kSun_center[0]));
static Wielobok WLudzik2(kLudzik2,sizeof(kLudzik2)/sizeof(kLudzik2[0]));
static Wielobok WLudzik3(kLudzik3,sizeof(kLudzik3)/sizeof(kLudzik3[0]));
static Wielobok WLudzik4(kLudzik4,sizeof(kLudzik4)/sizeof(kLudzik4[0]));

//Dostêp Biblioteka podstawowych kszta³tów w przestrzeni nazw klasy Wielobok, 
//Zrobione jako statyczne metody bo to pozwala robiæ potem ró¿ne implementacje 
//przechowywania tych wieloboków, np. tworzyæ je dopiero gdy bêd¹ potrzebne
//lub œci¹gaæ z dysku. 
//TU "static" by³o w klasie i oznacza co innego bo odnosi siê do metody!
const Wielobok&  Wielobok::Surface() {return WSurface;}
const Wielobok&  Wielobok::Center_plant(){return WCenter_plant;}
const Wielobok&  Wielobok::Plant() {return WPlant;}
const Wielobok&  Wielobok::Sun() {return WSun;}
const Wielobok&  Wielobok::Sun_center() {return WSun_center;}
const Wielobok& Wielobok::Ludzik(int typ) 
{
                                     cout<<"Ludzik-Typ:"<<typ<<endl;
      switch(typ){
      case 4: return WLudzik4;
      case 3: return WLudzik3;
      case 2: return WLudzik2;
      case 1: return WLudzik3;
      default:  return WLudzik3;
      }
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

Wielobok::Wielobok()
//Ale jest konstruktor bezparametrowy bo na upartego mo¿e byæ "zerowy"
{
   Punkty=new ssh_point[1];   
   Ilobok=0;
   Punkty[0].x=Punkty[0].y=0;
}

//Operator przypisanie Wieloboku - lepiej jakby nie mysia³o byæ, ale có¿...
Wielobok& Wielobok::operator = (const Wielobok& Wzorek)
{ 
  if(Ilobok!=Wzorek.Ilobok)
  {
     delete []Punkty;  
     Punkty=new ssh_point[Wzorek.Ilobok];                  assert(Punkty!=NULL);
  }    
  Ilobok=Wzorek.Ilobok;
  memcpy(Punkty,Wzorek.Punkty,Ilobok*sizeof(ssh_point));  
  return *this;
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
   if(Ilobok>2)
     fill_poly(StartX,StartY,Punkty,Ilobok,Color);  
}

const ssh_point&  Wielobok::DajPunkt(unsigned Ktory) const
{
    return Punkty[Ktory];
}

         

