///////////////////////////////////////////////////////////////////////////////////////////
//							Przykladowy program SYMSHELLA
//-----------------------------------------------------------------------------------------
//Obliczenia z jednoczesna wizualizacja. Algorytm genetyczny szuka maksimum zlozonej 
//funkcji trygonometrycznej ktorej wspolczynniki podawane sa z linii komend
//Przerwanie programu jest ignorowane za pierwszym razem - przerywa jedynie obliczenia.
///////////////////////////////////////////////////////////////////////////////////////////
#define USES_ASSERT
#define USES_STDIO
#define USES_STDLIB
#define USES_STRING
#define USES_CTYPE
#define USES_SYMULATION_SHELL
#define USES_MATH
#include "../../INCLUDE/uses_wb.hpp"
#include "../../INCLUDE/wb_ptr.hpp"

#define USES_STDC_RAND
#include "../../INCLUDE/random.hpp"

using namespace std;
using namespace wbrtm;

int WB_error_enter_before_clean=0; /* For close_plot() */
const hor=750;
const ver=550;
const LINECOLOR=128;
const double ZMIN=0;
const double ZMAX=M_PI*2;
const int MAX_POP=100;
const int PORCJA_KROKOW=10;

wb_dynarray<double> wspolczynniki;//Tablica wspolczynnikow fali. Poczatkowo calkiem pusta

double CalcFunction(unsigned long gen,unsigned long maxgen,double min,double max)
{
double par=double(gen)/(double(maxgen));
double pom=0;

assert(par<=1);
par=min+par*(max-min);
size_t IloscHarmonik=wspolczynniki.get_size();

for(size_t j=0;j<IloscHarmonik;j+=2) 
		{
		pom+=wspolczynniki[j]*cos((j/2+1)*par)+wspolczynniki[j+1]*sin((j/2+1)*par);
		}

assert(pom<=1 && pom>=-1);
return pom;
}

class genom
{
double fitness;
unsigned long gen;

protected:
static unsigned long mutuj(long gen)
{
size_t pos=RANDOM(320);
if(pos>=32) return gen; //NIE MA WSP. MUTACJI == jest zawsze 10%
// FLIP/FLOP jednego bitu
// Moze mozna to zrobic lepiej ale chyba mialem zacmienie
unsigned long maska=0x1<<pos;
if(gen & maska) // nie jest 0 - bit ustawiony
		gen&=~maska;//ZOSTAWIA TYLKO INNE
        else
		gen|=maska;// USTAWIA BIT 
return gen;
}

public:
static double maxfit;
static double maxgen;
static unsigned radius;

void reset()
{
fitness=MAXDOUBLE;
gen=RANDOM(ULONG_MAX);
}

genom():fitness(MAXDOUBLE)
{
gen=RANDOM(ULONG_MAX);
}

genom& operator = (const genom& sec)
{
fitness=MAXDOUBLE;
gen=mutuj(sec.gen);
return *this;
}

double get_fitness()
{
if(fitness==MAXDOUBLE)
	{
	fitness=CalcFunction(gen,ULONG_MAX,ZMIN,ZMAX);
	if(fitness>maxfit)
			{
			maxfit=fitness;
			maxgen=ZMIN+(double(gen)/double(ULONG_MAX))*(ZMAX-ZMIN);
			}
	}
return fitness;
}

unsigned long get_gen()
{
return gen;
}

void rysuj(int kolor)
{
	double pom=get_fitness();
	int y=ver-((pom+1)/2*ver);
	int x=(gen/(double(ULONG_MAX)))*hor;
	fill_circle(x,y-radius,radius,kolor);
}

void kasuj()
{
	double pom=get_fitness();
	int y=ver-((pom+1)/2*ver);
	int x=(gen/(double(ULONG_MAX)))*hor;
	fill_circle(x,y-radius,radius-1,0);
}

};

wb_dynarray<genom> populacja;

void init_gen_alg()
{
genom::maxfit=-MAXDOUBLE;
genom::maxgen=0;
genom::radius=7;
wb_dynarray<genom> initpop(MAX_POP); //resetuje populacje
populacja=initpop;//Zapamientuje globalnie
}

void gen_alg()
{
static int krok=0;
size_t A=RANDOM(MAX_POP);
size_t B=RANDOM(MAX_POP);
if(A==B) return; // NIE MA SENSU
genom& G_A=populacja[A];
genom& G_B=populacja[B];
if( G_A.get_fitness()>G_B.get_fitness() )
	{
	G_A.rysuj(32);
	G_B.kasuj();
	G_B=G_A;
	if( G_A.get_gen()!=G_B.get_gen() )
			G_B.rysuj(200);//MUTACJA!
			else
			G_B.rysuj(32);
	}
	else
	{
	G_B.rysuj(32);
	G_A.kasuj();
	G_A=G_B;
	if( G_A.get_gen()!=G_B.get_gen() )
			G_A.rysuj(200);//MUTACJA!
			else
			G_A.rysuj(32);
	}
}

void replot()
{
int TheEnd,licznik,kroki;
int oldm=mouse_activity(0);//Stary stan myszy
double old=0;			   //Poprzednia wartosc funkcji

INIT:
init_gen_alg();
TheEnd=0;
kroki=0;

BEGIN:
clear_screen();

//Rysowanie zlozonej funkcji trygonometrycznej
old=CalcFunction(0,hor,ZMIN,ZMAX);
for(unsigned i=1;i<hor;i++)
	{
	double pom=CalcFunction(i,hor,ZMIN,ZMAX);
	int y2=ver-((pom+1)/2*ver);
	int y1=ver-((old+1)/2*ver);
	line(i,y1,i,y2,LINECOLOR);
	old=pom;
	}

for(unsigned j=0;j<populacja.get_size();j++)
		populacja[j].rysuj(32);

//Oœ OX
line(0,ver/2,hor,ver/2,0);
line(hor,ver/2,hor-10,ver/2-10,0);
line(hor,ver/2,hor-10,ver/2+10,0);


//Algorytm genetyczny
licznik=0;
do{
print(0,0,"%d fit=%15.12f X=%15.12f",kroki,genom::maxfit,genom::maxgen);

gen_alg();//KROK algorytmu genetycznego

if((kroki++)%100==0) 
	{
	genom::radius--;
	if(genom::radius<3) genom::radius=3;
	}

flush_plot();

if(input_ready())//Sprawdzenie wejscia
	{
	int key=get_char();
	switch(key){
	case 'q':
	case 'Q':
	case EOF:TheEnd=1;goto END;
	case 'I':goto INIT;//Niejawnwq petla powtarzania obliczen FUJ!
    case '\r':goto BEGIN;//Niejawna petla odrysowywania FUJ!
	default:break;};
	}
	
if(++licznik>PORCJA_KROKOW)
		goto BEGIN;
}while(!TheEnd);

END:
//Koniec odrysowywania
mouse_activity(oldm);
}

/*  OGOLN¥ OBS£UGÊ ROBI FUNKCJA MAIN */
/*************************************/

main(int argc,char* argv[])
{
int i=0,xpos=0,ypos=0,click=0;//Myszowate
int cont=1;//flaga kontynuacji
int std=0;

mouse_activity(1);
set_background(255);
buffering_setup(1);/* Wlaczona animacja */
shell_setup("GENETIC ALGORITHM DEMONSTRATION",argc,argv);
printf("Use -help for SYMSHELL help page\n");

RANDOMIZE();

// Czytanie parametrow funkcji falowej
{
int licznik=0;
double suma_wsp=0;
wb_dynarray<double> initwspol(argc);

for(i=1;i<argc;i++)
	{
	char* stoper=NULL;
	double pomoc;
	pomoc=strtod(argv[i],&stoper);
	if(! ( *stoper=='\0' || isspace(*stoper) ) || argv[i]==stoper  )
		continue; //Nie cyfra - pewnie parametr symshell'a
	initwspol[licznik++]=pomoc;
	suma_wsp+=fabs(pomoc);
	}

if(licznik%2==1)//Jesli nieparzyscie to dodajemu do pary;
	licznik++;

if(licznik>0)
	wspolczynniki.alloc(licznik);
	else
	{
	fprintf(stderr,"USAGE: %s [list of any nymber of f.p. values]\n",argv[0]);
	fprintf(stderr,"Required at least one numerical parameter!\n");
	fprintf(stderr,"\nPRESS ENTER");
	getchar();
	exit(1);
	}

//initwspol i tak ma conajmniej jedna komorke za duzo (bo argc >=1 zasze)
initwspol[licznik-1]=0.0;//Jesli bylo parzyscie to sie pozniej zamaze

for(i=0;i<licznik;i++)
	{
	wspolczynniki[i]=initwspol[i]/suma_wsp;//Zeby w sumie wspolczynniki dawaly 1!	
	}

}

if(!init_plot(hor,ver,0,1))
		{
		printf("%s\n","Can't initialize graphics");
		exit(1);
		}

while(cont)//Petla zdarzen na czas pracy algorytmu
{
int tab;
if(input_ready())
   {
   tab=get_char();
   switch(tab)
		{
     	case '@':
		case '\n':
		case '\r':replot();break;//Sam algorytm jest jako replot() - nie polecam
	 	case 's':std=!std;break;
	 	case 'Q':
   	    case 'q':
		case EOF:cont=0;break;
		};
		if(std)
			{fflush(stdout);}
   flush_plot();
   }

}

close_plot();
printf("Do widzenia!!!\n");
return 0;
}


double genom::maxfit=-MAXDOUBLE;
double genom::maxgen=0;
unsigned genom::radius=7;
