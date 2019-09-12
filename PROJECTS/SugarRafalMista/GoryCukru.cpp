#include "symshell.h"
#include <stdio.h>
#include <iostream> 
#include <fstream> 
#include <cstdlib> 
using namespace std;

//ssh_point Pole[]={{0,0},{0,10},{10,10},{10,0}};

const unsigned okres=1000;  //liczba iteracji 
const int populacja=250; //liczba agentow 
const unsigned RANDOM_SEED=time(NULL);

short Swiat[51][51]; //plansza automatu komorkowego

short Torus(short x, short c); // wyliczanie wspolrzednych torusa - tylko deklaracja
class Agent { // klasa opisujaca agenta
private:
       short Xpoz; // wspolrzedna X
       short Ypoz; // wspolrzedna Y
       short Xcel; // wspolrzedna X celu ruchu agenta 
       short Ycel; // wspolrzedna Y celu ruchu agenta
       short wzrok; // ilosc komorek dostrzeganych przez agenta w czterech glownych kierunkach
       short zasoby; // wielkosc zasobow
       short metabolizm; // ile agent zjada w jednej turze
       short wiek; // ile maksymalnie agent moze zyc
       short rok; // zlicza, ile lat ma juz agent
       ssh_color Kolor;
public:
       short X(){return Xpoz;} // zwraca wspolrzedna X
       short Y(){return Ypoz;} // zwraca wspolrzedna Y
       short Z() {return zasoby;} // sprawdza, czy agent ma prawo zyc
       short W() {return wiek;} // zwraca wiek smierci
       short R() {return rok;} // zwraca wiek
       friend
       bool Kolizja(Agent& A,Agent& B); // sprawdza, czy w danym polu nie ma dwoch agentow
       bool CzyWolne(short x, short y); // sprawdza, czy pole jest zajete przez agenta 
       void PrzesunDo(short NowyX,short NowyY){Xpoz=NowyX;Ypoz=NowyY;} // przesuwa agenta
       // konstruktor bezparametrowy
       Agent(){
                Xpoz=rand()%50+1; // losowe rozmieszczenie agentow
                Ypoz=rand()%50+1;
                wzrok=rand()%6+1; // losowy wzrok
                zasoby=rand()%20+5;
                metabolizm=rand()%4+1;
                wiek=rand()%40+60; 
                rok=0;
                Kolor=60;
                }
        ~Agent()
                {}
       void Zaznacz(){ // rysuje agenta na bitmapie
            for(unsigned i=1;i<=6;i++) {plot(Xpoz+10*(Xpoz-1)+1+i,Ypoz+10*(Ypoz-1)+1,Kolor);}
            for(unsigned i=1;i<=6;i++) {plot(Xpoz+10*(Xpoz-1)+1+i,Ypoz+10*(Ypoz-1)+8,Kolor);}
            for(unsigned i=1;i<=8;i++)
                for(unsigned j=1;j<=6;j++) {plot(Xpoz+10*(Xpoz-1)+i,Ypoz+10*(Ypoz-1)+1+j,Kolor);}
           }   
       
       void Wybor(); // funkcja wyboru celu przez agenta
       void Praca(); // co robi agent na komorce
};   

class Cukier {
private:
        short urodzajnosc; // ile moze byc maksymalnie cukru w danej komorce
        short stopawzrostu; // ile cukru "odrasta" w danej turze
public:
       short U(){return urodzajnosc;} // zwraca urodzajnosc
       short W(){return stopawzrostu;} // zwraca stope wzrostu cukru
       Cukier() {
                urodzajnosc=0; // potem zostana przydzielone inne wartosci zgodnie z plansza Apsteina i Axtella
                stopawzrostu=1; // w wielu artykulach jest ona losowa
                }
       virtual ~Cukier()
             {}
       void Rozmiesc(short u); // przydziela urodzajnosci cukru wg planszy Apsteina i Axtella
};

Agent* Agenci;
Cukier* Komorka;

bool Kolizja(Agent& P,Agent& Q)
    {
    if(P.X()!=Q.X() || P.Y()!=Q.Y()) 
        return false;
    else return true;   
   }
  
bool Agent::CzyWolne(short x, short y) {
    for(unsigned i=0;i<populacja;i++)
    if( Agenci[i].X()==x && Agenci[i].Y()==y) return false;
    else return true;
}

void inicjalizacja()
    //ulozenie poczatkowe agentow
    {
       srand(time(NULL));       
       Agenci=new Agent[populacja]; // inicjalizacja agentów - wywolanie konstruktorow   
       
       for(unsigned i=0;i<populacja;i++) // sprawdzanie kolizji i-tego agenta i poprawa do skutku
       {                                           
         bool OK;                                        
         do
         { 
          OK=true; // domyslnie brak kolizji
          for(unsigned j=0;j<i;j++) 
            if( Kolizja(Agenci[i],Agenci[j]) )
            {                                                
              OK=false;// wykryta kolizja
              Agenci[i].PrzesunDo(rand()%50+1,rand()%50+1); // przesuniecie w losowa pozycje
              break; // sprawdzanie od nowa
            }  
          }while(!OK); // do skutku              
       }
    }     

void Agent::Wybor()
    {  
     Xcel=Xpoz;
     Ycel=Ypoz;               
     for(unsigned i=0;i<=wzrok;i++) {
     // porownuje najdalsze komorki z 4 kierunkow glownych z komorka poczatkowa, jesli znajdzie
     // wieksza lub rowna, to zamienia ja na poczatkowa - sprawdzane komorki przesuwaja sie ku srodkowi
     // i w ostatniej iteracji poczatkowa lokalizacja sprawdzana jest czterokrotnie
     // losowy dobor sprawdzanych kierunkow
     short Los[4];
     Los[0]=rand()%4;
     Los[1]=rand()%4;
     while(Los[0]==Los[1]) {Los[1]=rand()%4;}
     Los[2]=rand()%4;
     while(Los[0]==Los[2] || Los[1]==Los[2]) {Los[2]=rand()%4;}
     Los[3]=10-Los[0]-Los[1]-Los[2];
     for(unsigned h=0;h<=3;h++) {
   
        if (Los[h]==0) {
        if (Swiat[Ypoz][Torus(Xpoz,wzrok-i)]>=Swiat[Ycel][Xcel] && CzyWolne(Torus(Xpoz,wzrok-i),Ypoz))
          {Xcel=Torus(Xpoz,wzrok-i); Ycel=Ypoz;}
          }
        if (Los[h]==1) {
        if (Swiat[Torus(Ypoz,wzrok-i)][Xpoz]>=Swiat[Ycel][Xcel] && CzyWolne(Xpoz,Torus(Ypoz,wzrok-i)))
          {Xcel=Xpoz; Ycel=Torus(Ypoz,wzrok-i);}
          } 
        if (Los[h]==2) {
        if (Swiat[Ypoz][Torus(Xpoz,-wzrok+i)]>=Swiat[Ycel][Xcel] && CzyWolne(Torus(Xpoz,-wzrok+i),Ypoz))
          {Xcel=Torus(Xpoz,-wzrok+i); Ycel=Ypoz;}
          }
        if (Los[h]==3) {
        if (Swiat[Torus(Ypoz,-wzrok+i)][Xpoz]>=Swiat[Ycel][Xcel] && CzyWolne(Xpoz,Torus(Ypoz,-wzrok+i)))
          {Xcel=Xpoz; Ycel=Torus(Ypoz,-wzrok+i);}
          } 
    }
    }
    PrzesunDo(Xcel, Ycel); // w funkcji wbudowane jest przesuniecie agenta
    
    }
    
void Agent::Praca()
// wszystko co agent robi dostajac sie w wybrane przez siebie miejsce
{
     zasoby=zasoby+Swiat[Ypoz][Xpoz]; // agent zbiera caly cukier z komorki i dolacza do zasobow
     Swiat[Ypoz][Xpoz]=0; // komorka sie oproznia
     zasoby=zasoby-metabolizm; // czesc zasobow agent zjada
     rok++;       
    }

short Torus(short x, short c) { // przeliczanie wspolrzednych w torusie
     if (x+c>=1) {x=(x+c)%50;}
     if (x+c<1) {x=50+(x+c);}
return x;
}        
    
void krok()
    // ruchy agentow
    {       
       // przesuwanie agentow
       for(unsigned i=0;i<populacja;i++)
       {
        short PoprzedniX=Agenci[i].X();
        short PoprzedniY=Agenci[i].Y();
        Agenci[i].Wybor();   
        // sprawdzanie kolizji
        for(unsigned j=0;j<populacja;j++)
        {
          if(i!=j && Kolizja(Agenci[i],Agenci[j]) )
          {
           Agenci[i].PrzesunDo(PoprzedniX,PoprzedniY);
           break;
          }
        }
        Agenci[i].Praca();
        
        // jesli agent nie ma zasobow lub jest zbyt stary, to umiera i rodzi sie nowy
        if ((Agenci[i].Z()<0) || (Agenci[i].R()>Agenci[i].W())) {
           Agent* NowyAgent=new Agent[1];
           Agenci[i]=NowyAgent[0];
           }
                 
        }                                      
    }


void Cukier::Rozmiesc(short u) {
     urodzajnosc=u; // wygodna funkcja do rozmieszczania urodzajnosci cukru na planszy
}

void MapaCukru() // nadaje kolejnym komorkom urodzajnosc zgodnie z plansza Apsteina i Axtella
{
Komorka=new Cukier[2500]; 
 // poklady cukru na 2
for(unsigned i=17;i<=50;i++)
   for(unsigned j=1;j<=27;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(2);} // tablica Cukier jednowymiarowa dla oszczedzenia miejsca
for(unsigned i=1;i<=16;i++)
   for(unsigned j=19;j<=27;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(2);}    
     
//poklady cukru na 1
for(unsigned i=47;i<=50;i++)
   for(unsigned j=1;j<=4;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(1);}     
for(unsigned i=44;i<=46;i++)
   for(unsigned j=1;j<=2;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(1);}       
for(unsigned i=49;i<=50;i++)
   for(unsigned j=5;j<=7;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(1);}    
Komorka[(43-1)*50+1-1].Rozmiesc(1); Komorka[(50-1)*50+10-1].Rozmiesc(1); Komorka[(46-1)+3-1].Rozmiesc(1); Komorka[(48-1)*50+5-1].Rozmiesc(1); Komorka[(48-1)*6-1].Rozmiesc(1);
for(unsigned j=8;j<=10;j++) {Komorka[(50-1)*50+j-1].Rozmiesc(1);}
    
Komorka[(27-1)*50+1-1].Rozmiesc(1); 
for(unsigned i=17;i<=26;i++)
   for(unsigned j=1;j<=2;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(1);}  
for(unsigned i=16;i<=24;i++) {Komorka[(i-1)*50+3-1].Rozmiesc(1);}
for(unsigned i=15;i<=23;i++) {Komorka[(i-1)*50+4-1].Rozmiesc(1);}
for(unsigned i=15;i<=22;i++) {Komorka[(i-1)*50+5-1].Rozmiesc(1);}    
for(unsigned i=14;i<=22;i++) {Komorka[(i-1)*50+6-1].Rozmiesc(1);}
for(unsigned i=14;i<=21;i++) {Komorka[(i-1)*50+7-1].Rozmiesc(1);}
for(unsigned i=13;i<=20;i++) {Komorka[(i-1)*50+8-1].Rozmiesc(1);}
for(unsigned i=12;i<=19;i++)
   for(unsigned j=9;j<=11;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(1);} 
Komorka[(20-1)*50+9-1].Rozmiesc(1); Komorka[(11-1)*50+11-1].Rozmiesc(1); Komorka[(1-1)*50+27-1].Rozmiesc(1);


for(unsigned i=1;i<=2;i++)
   for(unsigned j=19;j<=26;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(1);} 
for(unsigned j=18;j<=25;j++) {Komorka[(3-1)*50+j-1].Rozmiesc(1);}
for(unsigned i=4;i<=9;i++)
   for(unsigned j=17;j<=23;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(1);}   
Komorka[(4-1)*50+25-1].Rozmiesc(1); 
for(unsigned i=4;i<=6;i++) {Komorka[(i-1)*50+24-1].Rozmiesc(1);}
for(unsigned i=10;i<=14;i++)
   for(unsigned j=13;j<=21;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(1);} 
Komorka[(16-1)*50+7-1].Rozmiesc(1); Komorka[(14-1)*50+9-1].Rozmiesc(1);
for(unsigned i=8;i<=9;i++)
   for(unsigned j=15;j<=16;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(1);} 
Komorka[(7-1)*50+16-1].Rozmiesc(1);
for(unsigned i=10;i<=12;i++) {Komorka[(i-1)*50+22-1].Rozmiesc(1);}
for(unsigned i=15;i<=17;i++)
   for(unsigned j=12;j<=18;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(1);}
for(unsigned i=11;i<=14;i++) {Komorka[(i-1)*50+12-1].Rozmiesc(1);}
for(unsigned i=15;i<=16;i++)
   for(unsigned j=18;j<=19;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(1);}
Komorka[(15-1)*50+20-1].Rozmiesc(1); Komorka[(18-1)*50+12-1].Rozmiesc(1); Komorka[(18-1)*50+13-1].Rozmiesc(1); Komorka[(18-1)*50+14-1].Rozmiesc(1);

// poklady cukru na 3
for(unsigned i=24;i<=26;i++)
   for(unsigned j=13;j<=22;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(3);} 
for(unsigned i=27;i<=42;i++)
   for(unsigned j=8;j<=23;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(3);}   
for(unsigned i=27;i<=39;i++)
   for(unsigned j=24;j<=25;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(3);}
for(unsigned i=29;i<=41;i++)
   for(unsigned j=6;j<=7;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(3);}
for(unsigned i=43;i<=44;i++)
   for(unsigned j=9;j<=21;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(3);}
for(unsigned i=31;i<=39;i++) {Komorka[(i-1)*50+5-1].Rozmiesc(3);}
for(unsigned i=28;i<=37;i++) {Komorka[(i-1)*50+26-1].Rozmiesc(3);}
Komorka[(28-1)*50+7-1].Rozmiesc(3); Komorka[(42-1)*50+7-1].Rozmiesc(3); Komorka[(43-1)*50+8-1].Rozmiesc(3); Komorka[(43-1)*50+22-1].Rozmiesc(3);
Komorka[(40-1)*50+24-1].Rozmiesc(3); Komorka[(41-1)*50+24-1].Rozmiesc(3);
for(unsigned j=11;j<=19;j++) {Komorka[(45-1)*50+j-1].Rozmiesc(3);}
for(unsigned j=9;j<=12;j++) {Komorka[(26-1)*50+j-1].Rozmiesc(3);}
for(unsigned j=23;j<=25;j++) {Komorka[(26-1)*50+j-1].Rozmiesc(3);}
Komorka[(25-1)*50+11-1].Rozmiesc(3); Komorka[(25-1)*50+12-1].Rozmiesc(3); Komorka[(25-1)*50+23-1].Rozmiesc(3); Komorka[(25-1)*50+24-1].Rozmiesc(3);

// poklady cukru na 4
for(unsigned i=31;i<=39;i++)
   for(unsigned j=11;j<=19;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(4);}
for(unsigned i=29;i<=30;i++)
   for(unsigned j=14;j<=17;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(4);}
for(unsigned i=33;i<=36;i++)
   for(unsigned j=20;j<=21;j++) {Komorka[(i-1)*50+j-1].Rozmiesc(4);}
for(unsigned i=32;i<=37;i++) {Komorka[(i-1)*50+10-1].Rozmiesc(4);}
for(unsigned j=13;j<=18;j++) {Komorka[(40-1)*50+j-1].Rozmiesc(4);}
Komorka[(30-1)*50+12-1].Rozmiesc(4); Komorka[(30-1)*50+13-1].Rozmiesc(4); Komorka[(30-1)*50+18-1].Rozmiesc(4); Komorka[(30-1)*50+19-1].Rozmiesc(4);
Komorka[(31-1)*50+20-1].Rozmiesc(4); Komorka[(32-1)*50+20-1].Rozmiesc(4); Komorka[(37-1)*50+20-1].Rozmiesc(4); Komorka[(38-1)*50+20-1].Rozmiesc(4);

// symetrycznosc gor
for(unsigned i=1;i<=45;i++)
   for(unsigned j=5;j<=27;j++) {Komorka[(46-i-1)*50+55-j-1].Rozmiesc(Komorka[(i-1)*50+j-1].U());}

// poklady cukru na 1 c. d.
for(unsigned j=20;j<=32;j++) {Komorka[(50-1)*50+j-1].Rozmiesc(1);}
for(unsigned j=23;j<=33;j++) {Komorka[(49-1)*50+j-1].Rozmiesc(1);}
for(unsigned j=24;j<=33;j++) {Komorka[(48-1)*50+j-1].Rozmiesc(1);}
for(unsigned j=26;j<=34;j++) {Komorka[(47-1)*50+j-1].Rozmiesc(1);}
for(unsigned j=27;j<=35;j++) {Komorka[(46-1)*50+j-1].Rozmiesc(1);}
Komorka[(40-1)*50+39-1].Rozmiesc(1);
}

void Wzrost() { // cukier odrasta w kazdej rundzie wg zadanej stopy
for(unsigned i=1;i<=50;i++)
   for(unsigned j=1;j<=50;j++) {
     if (Swiat[i][j]!=Komorka[(i-1)*50+j-1].U()) //czy zostal osiagniety poziom maksymalny
          {Swiat[i][j]+=Komorka[(i-1)*50+j-1].W();} // odrasta
     if (Swiat[i][j]>Komorka[(i-1)*50+j-1].U())
          {Swiat[i][j]=Komorka[(i-1)*50+j-1].U();} //nie moze przekraczac limitu urodzajnosci
   }
}     

void InicjujSwiat() { // poczatkowo komorki swiata wypelnione maksymalnymi ilosciami cukru
for(unsigned i=1;i<=50;i++)
   for(unsigned j=1;j<=50;j++) {Swiat[i][j]=Komorka[(i-1)*50+j-1].U();}
}

void Rysuj(short Swiat[51][51]) // rysuje plansze
{
 //przenoszenie komórek w pêtli na piksele bitmapy
 for(unsigned i=1;i<51;i++)
   for(unsigned j=1;j<51;j++)
   {for (unsigned e=0; e<=10;e++)
       	  for (unsigned f=0; f<=10; f++) {
              if (Swiat[i][j]==0) plot(j+e+10*(j-1),i+f+ 10*(i-1),255); // jedna komorka na bitmapie = 10 pikseli
              if (Swiat[i][j]==1) plot(j+e+10*(j-1),i+f+ 10*(i-1),250);
              if (Swiat[i][j]==2) plot(j+e+10*(j-1),i+f+ 10*(i-1),240);
              if (Swiat[i][j]==3) plot(j+e+10*(j-1),i+f+ 10*(i-1),230);
              if (Swiat[i][j]==4) plot(j+e+10*(j-1),i+f+ 10*(i-1),210);  
           }
   }  
}

void replot()
    // procedura odrysowujaca aktualny stan [TYLKO agentów. WB]
 	{        
      for(unsigned i=0;i<populacja;i++) 
                   {Agenci[i].Zaznacz();}
	  flush_plot();	// Bitmapa gotowa 
 	}

double gini(){
    double Zasoby[populacja];
    for(unsigned i=0;i<populacja;i++) {
    Zasoby[i]=Agenci[i].Z();
    }
        
    double s;
    for(unsigned u=1;u<=populacja;u++) 
    for(unsigned i=1;i<populacja;i++) {
    if (Zasoby[i-1]>Zasoby[i]){ // sortuje tablice rosnaco - potrzebne do wyliczenia wspolczynnika Giniego
    s=Zasoby[i-1];
    Zasoby[i-1]=Zasoby[i];
    Zasoby[i]=s;
    }}
    
    double suma=0;
    for(unsigned i=0;i<populacja;i++) {
    suma+=Zasoby[i];}
    double suma2=0;
    for(unsigned i=0;i<populacja;i++) {
    suma2+=(populacja-i)*Zasoby[i];}
    double g=0;
    double p=populacja;
    g=1+(1/p)-((2*suma2)/(populacja*suma)); // wylicza wspolczynnik Giniego
    return g;
}
   

int main(int argc,char* argv[])
{
    buffering_setup(1);/* Bêdzie pamietaæ w bitmapie zawartosc ekranu */
    shell_setup("GORY CUKRU",argc,argv);/* Przygotowanie okna  */
    init_plot(550,550,0,);/* Otwarcie odpowiedniego okna */
    MapaCukru();
    InicjujSwiat();
    Rysuj(Swiat);
    inicjalizacja();
    replot();
    flush_plot();
        
   for(unsigned t=0;t<=okres;t++)
    {
     gini();    
     cout.setf(ios::fixed);
     cout<<"Okres "<<t<<": wspolczynnik Giniego "<<gini()<<endl;       
     krok();
     clear_screen();
     
     Rysuj(Swiat); 
     
     if(input_ready()) //To musialem przerobiæ
     {
       switch(get_char()){
       case '\n':replot();
       case -1:
       case 27:
       case 'q':
       case 'Q':goto KONIEC;}
     }
     else replot();//Byl tylko replot
     
     print(0,0,"%d",t);
     flush_plot();
     
     Wzrost();                
    } 
    
KONIEC:       
    delete []Agenci;
    delete []Komorka;
    close_plot();
    printf("Do widzenia!\n");
 return 0;
}

