#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "symshell.h"

const char* PROGNAME="LAPUNOW v1.6a comp.:"__DATE__;
int NUMBER=350;

char outputname[100]="lapunow.$$$";/* nazwa zbioru z zapisem wynikow */
char pattern[100]="101";/* wzor strategi w funkcji lapunowa */
int np=sizeof(pattern)-1;/* i jego rozmiar */

double XS=0,YS=0;
double jedenx=0.0, jedeny=0.0;/* inicjacja jest zmieniana w init ! */
int my=0,mx=0,mc=0,NN=64;
FILE* trout;

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

void detailsALL(void)
{
register int y=0;
register int x=0;
register int c=0;
double xr=0.0,yr=0.0;
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
	 c=NN*lapunALL(xr,yr);
	 //fprintf(trout," c=%i ",c);
	 c=c%mc;
	 plot(x,y,c);
	}

	flush_plot();	
	if(input_ready())
		 if(get_char()==EOF)
						exit(2);
   }

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
char bufor[1000];
char command;
double X=4.0,Y=4.0;

buffering_setup(1);
fix_size(1);

if (!init_plot(800,600,0,0)) 
   {
      //printf("Can't initilize garphics output!\n Press enter to exit!");
	  fprintf(stderr,"Nie mozna otworzyc wyjscia graficznego!\n Wcisnij enter!");
      getch();
      exit(1);            
   }

my=screen_height();
mx=screen_width();
mc=256;
printf("\n");
printf(" RYSOWANIE WYKLADNIKA LAPUNOWA DLA ROWNANIA LOGISTYCZNEGO O ZMIENNEJ STRATEGII \n");
printf("-----------------------------------------------------------------------------------------");
printf("Wykladnik Lapunowa liczony jest dla iteracji rownania\n" 
	   "logistycznego o zmiennej strategi danego wzorem:\n");
printf(" St+1 = (Rx^Ry)*St*(1-St) \n");
printf("gdzie Rn w danej iteracji zadane jest cyklicznym wzorcem binarnym.\n");

do{
printf("\n");
//printf(" Grafika %d x %d\n ",mx,my);
printf(" Rozdzielczosc ekranu:  %d / %d pixeli \n %d kolorow\n",mx,my,mc);
printf(" X: %g..%g \n Y: %g..%g \n"
	   "(L)iczba iteracji: %d \n"
	   "(P)attern: %s[%d]\n"
	   //"(Z)bior wyjsciowy: %s\n"
	   ,XS,X,YS,Y,NUMBER,pattern,np
	   //,outputname
	   ); 
printf(" \nWcisnij x,X,y,Y,L,P lub ~ jako akceptacja !\n>>");
command=getche();
printf("\n");

switch (command) 
{
case 27 :printf("ESCAPE. OK. exit.");exit(0);
case 'X':printf("MAX X? ");gets(bufor);X=atof(bufor);break;
case 'x':printf("MIN X? ");gets(bufor);XS=atof(bufor);break;
case 'Y':printf("MAX Y? ");gets(bufor);Y=atof(bufor);break;
case 'y':printf("MIN Y? ");gets(bufor);YS=atof(bufor);break;
case 'L':printf("ITERACJI? ");gets(bufor);NUMBER=atof(bufor);  break;
case 'P':printf("STRATEGIA? ");gets(bufor);strcpy(pattern,bufor);break;   
case '`':break;
case '~':break;
//case 'Z':putchar('?');gets(bufor);strcpy(outputname,bufor);break;
default:printf("\n%c ?????",command);break;
}

np=strlen(pattern);
}while(!(command=='`' || command=='~') );

/*
   if ((trout = fopen(outputname, "wt"))
       == NULL)
   {
      fprintf(stderr, "Cannot open output \
	      file.\n");
      exit(1);
   }
  atexit(closing);
*/

  //jedenx=(double)mx/X;
  //jedeny=(double)my/Y;
  
  jedenx=(double)mx/(X-XS);
  jedeny=(double)my/(Y-YS);

  printf("LICZY SIE. PATRZ NA OKNO GRAFICZNE.\n");
  /*
  fprintf(trout,"REM Nie ogladac za pomoca prymitywnych edytorow !!!");
  fprintf(trout,"BEGIN\n maxX=%d\n maxY=%d\n oneofX=%g\n oneofY=%g\n",mx,my,jedenx,jedeny);
  fprintf(trout,"pattern %s\n",pattern);
  fflush(trout);
*/
  return ;
/* end initialization */
}

int main(int argc,char* argv[])
{ 
	shell_setup(PROGNAME,argc,argv);
  init();
  schem();
  //detailsBIN();
    detailsALL();          
   /* clean up */
	printf("POLICZONO. MOZNA JUZ TYLKO ZAMKNAC OKNO.\n");
   while(get_char()!=EOF);
   return 0;
}
