//Wielokrotna iteracja równania logistycznego - dla ró¿nych R
//z diagramem Feigenbauma w trybie graficznym, choæ bez solidnej obslugi zdarzeñ
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <cassert>
#include "SYMSHELL/symshell.h"

using namespace std;
int ile_na_c_pasek=4; //Jak d³uga linia dla pojedynczej korelacji

inline double iteracja(double X,double R)
{                      //Sprawdzenie za³o¿eñ
                                assert(0<=X && X<=1);
                                assert(0<=R && R<=4);
   return R*X*(1-X);  //Obliczenie kolejnej wartoœci                      
}

//Correlation 
double Corr(double PAM1[],double PAM2[],size_t ITMAXP) 
{
	int i;	
	double X_s=0,Y_s=0;
	double summ1=0,summ2=0,summ3=0,corelation=0;
	
	for(i = 0; i < ITMAXP; i++)
   	{
		X_s+=PAM1[i];
		Y_s+=PAM2[i];
   	}
	
	X_s/=ITMAXP;	
	Y_s/=ITMAXP;	
	
	for(i = 0; i < ITMAXP; i++)
   	{
		summ1+=(X_s-PAM1[i])*(Y_s-PAM2[i]);
		summ2+=(X_s-PAM1[i])*(X_s-PAM1[i]);
		summ3+=(Y_s-PAM2[i])*(Y_s-PAM2[i]);
   	}
   	
    if(summ2==0 || summ3==0)
      corelation=-9999;//Umownie, bo tak naprawdê nie da siê policzyæ
    else
	  corelation=summ1/(sqrt(summ2)*sqrt(summ3));
	                                           // assert(fabs(corelation)<=1.01);//+0.01 bo moga byc bledy floating-point
	return corelation;
}

void CorrColor(double C)
{
  if(C>0) set_pen_rgb(C*255,C*255,0,1,1);
  else      set_pen_rgb(0,-C*255,-C*255,1,1);
}

unsigned Vert=400*2;
unsigned Hori=200*2;  
double Xn[10000]; //Historia przebiegu
double Cn[20];    //Korelacje z przebiegu

int main(int argc,const char* argv[])//Potrzebne s¹ parametry wywo³ania programu 
{
    buffering_setup(1);// Czy bêdzie pamietaæ w bitmapie zawartosc ekranu? NA RAZIE MUSI
    shell_setup("Feigenbaum",argc,argv);// Przygotowanie okna 
    init_plot(Vert,Hori+8*(ile_na_c_pasek-1)+1,0,1);// Otwarcie okna 400x200 pixeli + 1 wiersz znakow pod pikselami
   
    ofstream Out("FeigANDautocorel.out");
    
    double Xstart=0; //Nadanie wartoœci niepoprawnych
    double Rstart=1; //Pocz¹tkowa wartoœæ R
    double Rkrok=0.001;//O ile zmieniamy wartoœæ R
    int ilepomin=100;//Kroki pomijane na pocz¹tku
    int ileiter=200; //kroki wizualizowane
    
    cout<<"Iteracja logistyczna w grafice "<<endl;
    cout<<"Ile krokow przedbiegu:";
    cin>>ilepomin;
    cout<<"Ile krokow do statystyk (nie wiêcej ni¿ 10000):";
    cin>>ileiter;    
    cout<<"X startowe (0..1, ale 0 oznacza losowe):";
    cin>>Xstart;
    cout<<"Pierwsze R (0..4):";
    cin>>Rstart;  
    cout<<"Krok zmiany R:";  
    cin>>Rkrok;
    
    for(double R=Rstart;R<=4.0;R+=Rkrok)
    {   
        double X=Xstart; //Ustawieni startowego X dla tego przebiegu   
        if(X==0)
           X=rand()/(RAND_MAX*1.0);
                
        for(int i=0;i<ilepomin;i++)
               X=iteracja(X,R);    //Policzenie nastêpnego
               
        //W³aœciwe dane
        double A=(R-Rstart)*(Vert/(4-Rstart));//Poziom to R - nie zmienia siê w pêtli
        //cout<<R<<' '<<A<<endl;
        
        for(int i=ilepomin;i<ilepomin+ileiter;i++) //Petla iterowania
        {
            double B=(1-X)*Hori;//Pion, aktualna wartoœæ X, obracamy ze wzglêdu na komputerowy uk³¹d wspó³rzêdnych
            plot_rgb(A,B,i%256,(i*2)%256,(i*4)%256); //KOLOROWANIE ZALEZNE OD KROKU   
            X=iteracja(X,R);    //Policzenie nastêpnego
            Xn[i-ilepomin]=X;   //Zapamiêtajmy historie 
        }
        
        Cn[1]=Corr(Xn,Xn+1,ileiter-1);
        Cn[2]=Corr(Xn,Xn+2,ileiter-2);
        Cn[3]=Corr(Xn,Xn+3,ileiter-3);
        Cn[4]=Corr(Xn,Xn+4,ileiter-4);
        Cn[5]=Corr(Xn,Xn+5,ileiter-5);
        Cn[6]=Corr(Xn,Xn+6,ileiter-6);
        Cn[7]=Corr(Xn,Xn+7,ileiter-7);
        Cn[8]=Corr(Xn,Xn+8,ileiter-8);
        Out<<R<<'\t'<<Cn[1]<<'\t'<<Cn[2]
              <<'\t'<<Cn[3]<<'\t'<<Cn[4]
              <<'\t'<<Cn[5]<<'\t'<<Cn[6]
              <<'\t'<<Cn[7]<<'\t'<<Cn[8]
              <<'\t'<<Xstart<<'\t'<<X<<endl;
              
        CorrColor(Cn[8]); line_d(A,Hori,                 A,Hori+  ile_na_c_pasek); 
        CorrColor(Cn[6]); line_d(A,Hori+  ile_na_c_pasek,A,Hori+2*ile_na_c_pasek); 
        CorrColor(Cn[4]); line_d(A,Hori+2*ile_na_c_pasek,A,Hori+3*ile_na_c_pasek); 
        CorrColor(Cn[2]); line_d(A,Hori+3*ile_na_c_pasek,A,Hori+4*ile_na_c_pasek); 
        CorrColor(Cn[1]); line_d(A,Hori+4*ile_na_c_pasek,A,Hori+5*ile_na_c_pasek); 
        CorrColor(Cn[3]); line_d(A,Hori+5*ile_na_c_pasek,A,Hori+6*ile_na_c_pasek); 
        CorrColor(Cn[5]); line_d(A,Hori+6*ile_na_c_pasek,A,Hori+7*ile_na_c_pasek); 
        CorrColor(Cn[7]); line_d(A,Hori+7*ile_na_c_pasek,A,Hori+8*ile_na_c_pasek);    
          
        flush_plot();//Obrazek na ekran
        input_ready();//¿eby "¿yæ", okno musi sprawdzaæ komunikaty, choæ mo¿e je ignorowaæ
    }   
                    
    cout<<"Zrobione"<<endl; 
    
    while(get_char()!=EOF);	// Tu czeka i ignoruje ewentualne zdarzenia 
    close_plot();           // Zamykamy okno - po zabawie 
     
    return 0;          //Zwyczajowy koniec funkcji main gdy OK
}

int WB_error_enter_before_clean=1; //"wysycenie" extern którego potrzebuje symshell