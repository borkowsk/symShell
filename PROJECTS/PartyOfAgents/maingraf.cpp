 	// RUCHOME LUDZIKI - program SYMSHELLA pokazuj¹cy idee ruchu celowego
    /////////////////////////////////////////////////////////////////////////////////// 
 	// POD Dev-Cpp potrzebne s¹ dwie bibloteki:
 	// ".../Dev-Cpp/lib/libgdi32.a" 
 	// oraz "...Dev-Cpp/lib/libcomdlg32.a"
 	
 	#include "symshell.h"
 	#include "Wieloboki.hpp"
 	#include <iostream>
 	#include <cstdlib>
 	#include <cmath>
 	using namespace std;
 	
 	const int size=500; //Bok obszaru roboczego
 	const int iluich=100; //ILU JEST AGENTÓW!
 	const int RAND_START=25;//A mo¿na np. time(NULL) - wtedy "bardziej losowo"
    
    class Agent
    {
      Wielobok  Ksztalt;
      double Xpoz;
      double Ypoz;
      unsigned Labilnosc; //Im wieksza wartosci tym czêsciej zmienia cel
      ssh_color Kolor;
      
      //Parametry celu do którego zmierza
      double Xcel;
      double Ycel;
      public:
      //Czytanie atrybutów
      double X(){return Xpoz;}
      double Y(){return Ypoz;}   
      //Obliczanie ograniczeñ prostok¹ta opisanego na wielok¹cie   
      double maxX(){ return Xpoz+Ksztalt.DajMaxX();}
      double minX(){ return Xpoz+Ksztalt.DajMinX();}
      double maxY(){ return Ypoz+Ksztalt.DajMaxY();}
      double minY(){ return Ypoz+Ksztalt.DajMinY();} 
      //Sprawdzanie kolizji z innym agentem
      friend //nie powinno byæ metod¹ agenta, bo jest symetryczne, st¹d "friend"
      bool Kolizja(Agent& A,Agent& B);
      //Zmiany atrybutów
      void UstawKolor(ssh_color nKolor){ Kolor=nKolor%256; } //Zabezpieczona zmiana kolorów
      void PrzesunDo(double NowyX,double NowyY){Xpoz=NowyX;Ypoz=NowyY;}//"Rêczne" przesuwanie agenta w inne miejsce
      
      //Konstruktor  bezparametrowy - konieczny jeœli maj¹ byæ w tablicy     
      Agent():Ksztalt( Wielobok::Ludzik(0) )//,Xpoz(0),Ypoz(0)
             {
                Xpoz=rand()%size;
                Ypoz=rand()%size;
                Xcel=rand()%size;
                Ycel=rand()%size;
                Labilnosc=rand()%5;
                Kolor=50+Labilnosc*40+rand()%20;//Pierwsze 50 kolorów jest raczej ciemne
                Ksztalt.Centruj();//¯eby œrodek by³ w œrodku :-)
                Ksztalt.Skaluj(1,1.5);//I wielkoœæ jakaœ rozs¹dna
             }
      //Destruktor - na razie nic jawnie nie robi
      //ale na wszelki wypadek wirtualny
      virtual ~Agent()
              {}     
      //Do wizualizacji        
      void Rysuj()
           {Ksztalt(Xpoz,Ypoz,Kolor);}    
      //Do symulacji     
      void ZrobKrok();     
    };
    
    Agent* Agenci;//"KOLEKCJA" AGENTÓW - NIE BÊD¥ ZNIKAÆ TO MO¯E BYÆ TE¯ ZWYK£A TABLICA 
    
    bool Kolizja(Agent& P,Agent& Q)
   // czy prostok¹ty AABB siê przecinaj¹
   {//Sprawdzenie czy przecinanie jest wykluczone
    if( P.minX() > Q.maxX() || P.maxX() < Q.minX()
     || P.minY() > Q.maxY() || P.maxY() < Q.minY() ) 
        return false; //Gdy prawd¹ jest ¿e siê NIE przecinaj¹
    else
    {
      //cout<<"Kolizja:"<<endl;
      //cout<<P.minX()<<' '<<P.maxX()<<" vs "<< Q.minX() <<' '<<Q.maxY()<<endl;
      //cout<<P.minY()<<' '<<P.maxY()<<" vs "<< Q.minY() <<' '<<Q.maxY()<<endl; 
      return true; //W przeciwnym przypadku
    }
   }
    
    void inicjalizacja()
    //U³o¿enie pocz¹tkowe i inne operacje konieczne do zrobienia na pocz¹tku
    {
       srand(RAND_START); //Inicjalizacja generatora      
       Agenci=new Agent[iluich]; //Inicjalizacja agentów - wywo³uj¹ siê konstruktory   
       
       for(unsigned i=0;i<iluich;i++) //Sprawdzamy kolizje i-tego agenta i poprawiamy do skutku
       {                                           cout<<i<<": ";
         bool OK; //Deklarowane tu bo musi byæ widoczne poza pêtl¹ do{}while()                                        
         do
         { 
          OK=true; //Domyœlnie nie ma kolizji, chyba ¿e wykryjemy dalej
          for(unsigned j=0;j<i;j++) //Tylko z tymi "po spodem" (wczeœniejszymi)
            if( Kolizja(Agenci[i],Agenci[j]) )
            {                                       cout<<"kz:"<<j<<' ';         
              OK=false; //Wykryto kolizje
              Agenci[i].PrzesunDo( rand()%size, rand()%size ); //Przesuwamy w losow¹ pozycje
              break;  //Jedna kolizja wystarczy ¿eby by³o nie-OK i sprawdzanie od nowa
            }  
          }while(!OK);  /*A¿ siê uda */              cout<<endl;
       }
    }     
    
    void Agent::ZrobKrok()
    {   
       //Zabezpieczenie przed wy³a¿eniem poza "pole walki" - niepe³ne...
       //Na razie wiemy ¿e cel nigdy nie losuje siê poza ekranem  
       if(Xpoz<Xcel) Xpoz++;
          else Xpoz--;
       if(Ypoz<Ycel) Ypoz++;
          else Ypoz--;    
       //Czasami cel siê zmienia losowo. 
       if(rand()%(size/(1+Labilnosc))==0) //TODO - prawdopodobieñstwo zmiany mo¿e byæ cech¹ indywidualn¹!
       {
          Xcel=rand()%size;
          Ycel=rand()%size;               
       }  
    }
    
    void krok()
    //Zmiany u³o¿enia zwi¹zane z ró¿nymi ruchami
    {       
       //Przesuwanie
       for(unsigned i=0;i<iluich;i++)
       {
        double PoprzedniX=Agenci[i].X();
        double PoprzedniY=Agenci[i].Y();
        Agenci[i].ZrobKrok();   
        //Sprawdzanie kolizji agenta z innymi
        for(unsigned j=0;j<iluich;j++)
        {
          if(i!=j && Kolizja(Agenci[i],Agenci[j]) )
          {
           Agenci[i].PrzesunDo(PoprzedniX,PoprzedniY);//Wycofaj ruch
           break;//Przerwanie pêtli sprawdzania, gdy ruch wycofany
          }
        }
       }                                     
    }
   
    void replot()
    //Procedura odrysowujaca aktualny stan
 	{
      clear_screen();//Czyszczenie ca³ego ekranu na wszelki wypadek
      //TODO:UPORZ¥DKOWANIE WG. Y-kow, ZEBY SIÊ DOBRZE ZAS£ANIA£Y 
      // qsort(...), 
      /* void qsort(
              void *base,
              size_t num,
              size_t width,
              int (__cdecl *compare )(const void *, const void *) 
       );*/
      //ale bezpoœrednio u¿yty na tablicy Agenci by³by skrajnie nieefektywny,  
      //bo by musia³ przestawiaæ w pamiêci ca³ych agentów!
      //Trzeba zmieniæ arganizacjê œwiata na tablice wskaŸników, albo zrobiæ
      //tak¹ tablicê wskaŸników lokalnie
      
      //TERAZ MO¯NA JU¯ RYSOWAÆ
      for(unsigned i=0;i<iluich;i++)
        Agenci[i].Rysuj();
        
 	  flush_plot();	// Bitmapa gotowa 
 	}

    void statystyka()
    {
         //...NA POTEM
    }

 	//Zmienne do obs³ugi zdarzeñ i dalej funkcje które to robi¹, w tym main()
 	const int WAIT=10; //Co ile nawrótów pêli zmieniaæ kolor
    const int DELA=100/WAIT;//Oczekiwanie w obrêbie pêtli
 	const char* CZEKAM="..."; 
 	unsigned long loop=0;

 	void read_mouse() //Procedura obslugi "kliku" myszy
 	{ 
       int xpos,ypos,click;
       if(get_mouse_event(&xpos,&ypos,&click)!=-1)//Operator & - pobranie adresu
	   {
          //.....x=xpos;
          //.....y=ypos;
          replot();                                                   
       }                      
 	}

 	int main(int argc,char* argv[])//Potrzebne s¹ paramatry wywo³ania programu
 	{
       mouse_activity(1);       //Czy mysz bêdzie obs³ugiwana   
       buffering_setup(1);      //Czy pamietac w bitmapie zawartosc ekranu TAK
 	   fix_size(1);	            //Czy symulowaæ niezmiennosci rozmiarow okna TAK
 	   shell_setup("SYMULACJA AGENTOWA",argc,argv);//Ustalenie tytu³u i konfiguracji z linii komend
 	   init_plot(size,size,0,1);// size x size pixeli + 1 wiersz znakow pod spodem
 	   
 	   inicjalizacja();         //Przygotowanie paramerów
 	   
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
      statystyka(); //Obliczanie chwilowych statystyk
      replot();  //Rysowanie   
      if(loop%WAIT<WAIT/2)
         printc(0,screen_height()-char_height('C'),128,255,"%u %s",loop,CZEKAM);
      else
        printc(0,screen_height()-char_height('C'),255,28,"%u %s",loop,CZEKAM);       
 	  flush_plot();// Bitmapa po modyfikacji ju¿ gotowa   
 	  delay_ms(DELA);//Wymuszenie oczekiwania - ¿eby pêtla nie zjada³a ca³ego procesora  
                 //I ¿eby siê wszystko nie dzia³o zbyt szybko!
 	  } 
 	}

 	cout<<"Dealokacja tablicy agentów"<<endl;
 	delete []Agenci; //Taka dziwna sk³adnia, bo wywo³uje destruktory!

 	cout<<"Wykonano "<<loop<<" obrotow petli.\nDo widzenia!\n"<<endl;
 	close_plot(); //Po zabawie - zamykam okno
 	return 0;
 	}
