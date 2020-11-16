#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include "M_APM.h"
#include "symshell.h"

const char* PROGNAME="LAPUNOW v2a comp.: "__DATE__;//with MAPM arbitrally precision math library
int NUMBER=350;

char outputname[100]="lapunow.$$$";/* nazwa zbioru z zapisem wynikow */
char pattern[1000]="10";/* wzor strategi w funkcji lapunowa */
int np=2;/* i jego dlugosc - musi sie tu zgadzac z poprzednia linia*/

double XS=0,YS=0;
double jedenx=0.0, jedeny=0.0;/* inicjacja jest zmieniana w init ! */
int my=0,mx=0,mc=0,NN=64;
int obcinac=0;
FILE* trout;

int cscale=0;//0 domyslna, 1 szarosc, 2 dwakolory
void my_scale()
{
	int i=0;

	switch(cscale){
	case 1:
		for(i=1;i<255;i++)
			set_rgb(i,i,i,i);
		break;
	case 2:
		for(i=1;i<255;i++)
			set_rgb(i,255-i,0,i);	
		break;
	case 4:
		printf("NIE MOZNA PRZYWROCIC DOMYSLNEJ SKALI KOLOROW.\n");
		cscale=0;//Nie przywraca, ale po co ma wiele razy pisac
		break;
	case 0:
	default:
		//Domyslna - nic nie robic
		break;
	}
	
}
/*
double lapunMAPM(double a, double b)
// szacowanie funkcji lapunowa przy zmiennej strategi 
{
	MAPM total=0.0,wx=0.0,aktr=0.0,aktrWx=0.0,logz2=0.0;
	register int  i=0;
	
	logz2=log10(2.0);
	wx=0.0000000001;
	if(a==0.0) return 1.0;
	if(b==0.0) return 1.0;
	for(i=1;i<=NUMBER;i++)
	{
		if(pattern[i%np]=='0') aktr=a;
		else aktr=b;
		// x <- rx(1-x) funkcja logistyczna 
		aktrWx=aktr*wx;  
		wx=aktrWx*(1-wx);
		
		if (wx>3.4E38) { //fprintf(trout,"\n a aktr=%f wx=%f ",aktr,wx);
			return 0.0 ;}
		if (wx<-3.4E38){ //fprintf(trout,"\n a aktr=%f wx=%f ",aktr,wx);
			return 1.0 ;}
		
		aktr=fabs(aktr-2.0*aktrWx);
		
		if (aktr>3.4E38) { //fprintf(trout,"\n b aktr=%f wx=%f ",aktr,wx);
			return 0.0 ;}
		if (!(aktr>0)){ //fprintf(trout,"\n b0 aktr=%f wx=%f ",aktr,wx);
			return 1.0 ;}
		
		aktr=log10(aktr);
		
		if (aktr>3.4E38) { //fprintf(trout,"\n c aktr=%f wx=%f ",aktr,wx);
			return 0.0 ;}
		if (aktr<-3.4E38){ //fprintf(trout,"\n c aktr=%f wx=%f ",aktr,wx);
			return 1.0 ;}
		aktr=aktr/logz2;
		
		if (aktr>3.4E38) { //fprintf(trout,"\n d aktr=%f wx=%f ",aktr,wx);
			return 0.0 ;}
		if (aktr<-3.4E38){ //fprintf(trout,"\n d aktr=%f wx=%f ",aktr,wx);
			return 1.0 ;}
		
		total=total+aktr;
		
		if (total>3.4E38) { //fprintf(trout,"\n t aktr=%f total=%f ",aktr,total);
			return 0.0;}
		if (total<-3.4E38){ //fprintf(trout,"\n t aktr=%f total=%f ",aktr,total);
			return 1.0;}
	}
	return (total/(i-1));//end lapunow
}
*/

double lapunALL(double a, double b)
/* szacowanie funkcji lapunowa przy zmiennej strategi */
{
	double total=0.0,wx=0.0,aktr=0.0,aktrWx=0.0,logz2=0.0;
	register int  i=0;
	
	logz2=log10(2.0);
	wx=0.0000000001;
	if(a==0.0) return 1.0;
	if(b==0.0) return 1.0;
	for(i=1;i<=NUMBER;i++)
	{
		if(pattern[i%np]=='0') aktr=a;
		else aktr=b;
		/* x <- rx(1-x) funkcja logistyczna */
		aktrWx=aktr*wx;  
		wx=aktrWx*(1-wx);
		
		if (wx>3.4E38) { //fprintf(trout,"\n a aktr=%f wx=%f ",aktr,wx);
			return 0.0 ;}
		if (wx<-3.4E38){ //fprintf(trout,"\n a aktr=%f wx=%f ",aktr,wx);
			return 1.0 ;}
		
		aktr=fabs(aktr-2.0*aktrWx);
		
		if (aktr>3.4E38) { //fprintf(trout,"\n b aktr=%f wx=%f ",aktr,wx);
			return 0.0 ;}
		if (!(aktr>0)){ //fprintf(trout,"\n b0 aktr=%f wx=%f ",aktr,wx);
			return 1.0 ;}
		
		aktr=log10(aktr);
		
		if (aktr>3.4E38) { //fprintf(trout,"\n c aktr=%f wx=%f ",aktr,wx);
			return 0.0 ;}
		if (aktr<-3.4E38){ //fprintf(trout,"\n c aktr=%f wx=%f ",aktr,wx);
			return 1.0 ;}
		aktr=aktr/logz2;
		
		if (aktr>3.4E38) { //fprintf(trout,"\n d aktr=%f wx=%f ",aktr,wx);
			return 0.0 ;}
		if (aktr<-3.4E38){ //fprintf(trout,"\n d aktr=%f wx=%f ",aktr,wx);
			return 1.0 ;}
		
		total=total+aktr;
		
		if (total>3.4E38) { //fprintf(trout,"\n t aktr=%f total=%f ",aktr,total);
			return 0.0;}
		if (total<-3.4E38){ //fprintf(trout,"\n t aktr=%f total=%f ",aktr,total);
			return 1.0;}
	}
	return (total/(i-1));
	/*end lapunow*/
}

double lapunBIN(double a, double b)
/* szacowanie funkcji lapunowa przy zmiennej strategi */
{
	double total=0.0,wx=0.0,aktr=0.0,aktrWx=0.0,logz2=0.0;
	register int  i=0;
	
	logz2=log10(2.0);
	wx=0.0000000001;
	if(a==0.0) return 1.0;
	if(b==0.0) return 1.0;
	for(i=1;i<=NUMBER;i++)
	{
		if(pattern[i%np]=='0') aktr=a;
		else aktr=b;
		/* x <- rx(1-x) funkcja logistyczna */
		aktrWx=aktr*wx;  
		wx=aktrWx*(1-wx);
		
		if (wx>3.4E38) return 0.0 ;
		if (wx<-3.4E38) return 1.0 ;
		
		aktr=fabs(aktr-2.0*aktrWx);
		
		if (aktr>3.4E38) return 0.0 ;
		if (!(aktr>0)) return 1.0 ;
		
		aktr=log10(aktr);
		
		if (aktr>3.4E38) return 0.0 ;
		if (aktr<-3.4E38) return 1.0 ;
		
		aktr=aktr/logz2;
		
		if (aktr>3.4E38) return 0.0 ;
		if (aktr<-3.4E38) return 1.0 ;
		
		total=total+aktr;
		
		if (total>3.4E38) return 0.0;
		if (total<-3.4E38) return 1.0;
	}
	return (total/(i-1));
	/*end lapunow*/
}

double lapunLN(double a, double b)
/* szacowanie funkcji lapunowa przy zmiennej strategi */
{
	double total=0.0,wx=0.0,aktr=0.0,aktrWx=0.0,logz2=0.0;
	int  i=0;
	
	if(a==0) return 1.0;
	if(b==0) return 1.0;
	logz2=log(2.0);
	wx=0.0000000001;
	for(i=1;i<=NUMBER;i++)
	{
		if(pattern[i%np]=='0') aktr=a;
		else aktr=b;
		/* x <- rx(1-x) funkcja logistyczna */
		aktrWx=aktr*wx;  
		wx=aktrWx*(1-wx);
		
		if (wx>3.4E38) return 0.0;
		if (wx<-3.4E38) return 1.0;
		aktr=aktr-2.0*aktrWx;
		aktr=fabs(aktr);
		aktr=log(aktr)/logz2;
		total=total+aktr;
		
		if (total>3.4E38) return 0.0;
		if (total<-3.4E38) return 1.0;
	}
	return (total/(i-1));
	/*end lapunow LN*/
}

void schem(void)
{
	register int y=0;
	register int x=0;
	register int c=0;
	double xr=0.0,yr=0.0;
	
	for(y=1;y<my;y+=10)
	{
		for(x=1;x<mx;x+=10)
		{               
			plot(x,y,1);
			xr=(double)x/jedenx+XS;
			yr=(double)y/jedeny+YS;
			c=NN*lapunBIN(xr,yr);
			c=c%mc;
			plot(x,y,c);
		}
		
		if(input_ready())
			if(get_char()==EOF)
				exit(2);
	}
	return;
}

double val_min=DBL_MAX;	//Wrtosci do komunikacji
double val_max=-DBL_MAX;
int MM=64;

double my_max(double a,double b)
{
	return (a>b?a:b);
}

void detailsALL(void)
{
	register int y=0;
	register int x=0;
	register int c=0;
	double xr=0.0,yr=0.0;
	
	double val=0;
	val_min=DBL_MAX;
	val_max=-DBL_MAX;
	
	//fprintf(trout,"ALL:\n");
	for(y=1;y<my;y++)
	{
		
		for(x=1;x<mx;x++)
		{
			plot(x,y,1);
			//fprintf(trout,"\n x=%i y=%i ",x,y);
			// fflush(trout);
			xr=(double)x/jedenx+XS;
			yr=(double)y/jedeny+YS;
			val=lapunALL(xr,yr);
			if(val_min>val)
				val_min=val;
			if(val_max<val)
				val_max=val;

			if(obcinac && val<0)
			{
				c=0;
			}
			else
			{
				if(-0.001<val && val<0.001)
					c=mc-1;//Bialy
				else
				//	c=0;
					c=(int)(NN*val)%mc;
			}
			
			plot(x,y,c);
		}
		
		flush_plot();	
		if(input_ready())
		{
			int c=get_char();
			if(c=='\r')//Zmiana rozmiaru okna 
			{
				set_char('\r');//Wymusza odczytanie rozmiarow
				return;		// i wraca do glownego menu
			}
			else
			if(c==27)
				return;//przerwanie
			else
			if(c==EOF)
				exit(2);//zakonczenie
		}
	}

	MM=(int)(val_max+1);//Najblizszy zakres calkowitoliczbowy obejmujacy min i max
	MM=(int)(val_min-1);
	MM=my_max(	fabs((int)(val_max+1)),fabs((int)(val_min-1)) );//i tak 0==czarny czyli zerowy kolor
	MM=mc/MM;
	printf("ZAKONCZONO. Wykladnik w zakresie %g do %g. Proponowane M=%d\n",val_min,val_max,MM);
	return;
}

void detailsBIN(void)
{
	register int y=0;
	register int x=0;
	register int c=0;
	double xr=0.0,yr=0.0;
	//fprintf(trout,"BIN 256:\n");
	for(y=1;y<my;y++)
    {
		
		for(x=1;x<mx;x++)
		{
			plot(x,y,1);
			xr=(double)x/jedenx+XS;
			yr=(double)y/jedeny+YS;
			c=lapunBIN(xr,yr);
			//fprintf(trout,"%c",c%256);
			c=c%mc;
			plot(x,y,c);
		}
		
		flush_plot();
		if(input_ready())
			if(get_char()==EOF)
				exit(2);
			
			//fprintf(trout,"\n");
			//fflush(trout);
    }
	return;
}

void closing(void)
{
	//fprintf(trout,"\nEND"); 
	//fclose(trout);
	close_plot();
	return ;
}

void init(void)
{	
	int Koniec=0;//Sterowanie zakonczeniem z menu
	char bufor[1000];
	char command;
	double X=4.0,Y=4.0;
	
	buffering_setup(1);
	fix_size(0);
	
	if (!init_plot(400,400,0,0)) 
	{
		//printf("Can't initilize garphics output!\n Press enter to exit!");
		fprintf(stderr,"Nie mozna otworzyc wyjscia graficznego!\n Wcisnij enter!");
		getch();
		exit(1);            
	}

	if(input_ready())get_char();//Skip first \r
	
	my=screen_height();
	mx=screen_width();
	mc=256;
	printf("\n");
	printf(" RYSOWANIE WYKLADNIKA LAPUNOWA DLA ROWNANIA LOGISTYCZNEGO O ZMIENNEJ STRATEGII \n");
	printf("-------------------------------------------------------------------------------\n");
	printf(" Wykladnik Lapunowa liczony jest dla iteracji rownania\n" 
		   " logistycznego o zmiennej strategi danego wzorem:\n\n");
	printf("              St+1 = (Rx^Ry)*St*(1-St)           \n\n");
	printf(" gdzie Rn w danej iteracji zadane jest cyklicznym wzorcem binarnym.\n");
	

	do{
		printf("\n");
		//printf(" Grafika %d x %d\n ",mx,my);
		printf(" Rozdzielczosc ekranu:  %d / %d pixeli \n %d kolorow, mnoznik M=%d\n",mx,my,mc,NN);
		printf(" X: %g..%g \n Y: %g..%g \n"
			"(L)iczba iteracji: %d \n"
			"(M)noznik wizualizacyjny { M*lap(x,y) }\n"
			"(P)attern: %s[%d]\n"
			//"(Z)bior wyjsciowy: %s\n"
			,XS,X,YS,Y,NUMBER,pattern,np
			//,outputname
			); 
		printf(" \nWcisnij x,X,y,Y,L,P,o,g,d lub ENTER !\n>>");
		
		command=0;//Dwuokienne oczekiwanie na komendy
		do{
			if(input_ready())
			{			
				command=get_char();//From graphics window
				printf("%c #from graphics window\n",command);
			}
			else
				if(kbhit())
				{
					command=getche();
					if(command=='\r') command='\n';//'\r \' tylko z wejscia graficznego
					printf("\n");
				}
		}while(!command);
		
		
		switch (command) 
		{
		case 3 :printf("Ctrl-C. OK. exit.");exit(0);
		case 'o':printf("obcinac=%c?",(obcinac?'T':'N'));*bufor=getche();obcinac=(*bufor=='T'||*bufor=='t');break;
		case 'g':printf("szarosci=%c?",(cscale==1?'T':'N'));*bufor=getche();cscale=((*bufor=='T'||*bufor=='t')?1:4);break;
		case 'd':printf("dwukolorowa=%c?",(cscale==2?'T':'N'));*bufor=getche();cscale=((*bufor=='T'||*bufor=='t')?2:4);break;
		case 'm':
		case 'M':printf("M? ");gets(bufor);NN=atof(bufor);break;
		case 'X':printf("MAX X? ");gets(bufor);X=atof(bufor);break;
		case 'x':printf("MIN X? ");gets(bufor);XS=atof(bufor);break;
		case 'Y':printf("MAX Y? ");gets(bufor);Y=atof(bufor);break;
		case 'y':printf("MIN Y? ");gets(bufor);YS=atof(bufor);break;
		case 'l':
		case 'L':printf("ITERACJI? ");gets(bufor);NUMBER=atof(bufor);  break;
        case 'p':
		case 'P':printf("STRATEGIA? ");gets(bufor);strcpy(pattern,bufor);np=strlen(pattern);break;   
		case '\r': //Odrysowanie okna graficznego
					my=screen_height();
					mx=screen_width(); 
					break;
		case '\n':
		case '`':
		case '~':	jedenx=(double)mx/(X-XS);
					jedeny=(double)my/(Y-YS);
					printf("LICZY SIE. PATRZ NA OKNO GRAFICZNE.\n");
					my_scale(cscale);
					schem();
					//detailsBIN();
					detailsALL();      
					break;
		case 27:printf("ESC pressed.\n");break;
		case EOF: Koniec=1;
				break;
			//case 'Z':putchar('?');gets(bufor);strcpy(outputname,bufor);break;
		default:printf("\n%c ?????",command);break;
		}
		
		
	}while(!Koniec );
	
	/*
	if ((trout = fopen(outputname, "wt"))
	== NULL)
	{
	fprintf(stderr, "Cannot open output \
	file.\n");
	exit(1);
	}
	atexit(closing);

	fprintf(trout,"REM Nie ogladac za pomoca prymitywnych edytorow !!!");
	fprintf(trout,"BEGIN\n maxX=%d\n maxY=%d\n oneofX=%g\n oneofY=%g\n",mx,my,jedenx,jedeny);
	fprintf(trout,"pattern %s\n",pattern);
	fflush(trout);
	*/

	return ;
}

int main(int argc,char* argv[])
{ 
	shell_setup(PROGNAME,argc,argv);
	
	init();
    
	/* clean up */
	//printf("ZAKONCZONO. MOZNA JUZ TYLKO ZAMKNAC OKNO.\n");
	//while(get_char()!=EOF);
	return 0;
}
