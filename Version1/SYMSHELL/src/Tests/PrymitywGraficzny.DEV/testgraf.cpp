////////////////////////////////////////////////////////////////////////////////
//							Przykladowy program SYMSHELLA
//------------------------------------------------------------------------------
// Bardzo prosty test grafiki bez funkcji odtwarzania ekranu. 
// Nastepuje odtwarzanie z bitmapy 
// ale po zmianie rozmiaru okna ekran bedzie czarny!
// POD Dev-Cpp potrzebne s¹ dwie bibloteki:
// ".../Dev-Cpp/lib/libgdi32.a" oraz "...Dev-Cpp/lib/libcomdlg32.a"
////////////////////////////////////////////////////////////////////////////////
#include "symshell.h"
#include <stdio.h>
const int size=16;

int main(int argc,char* argv[])//Potrzebne s¹ paramatry wywo³ania programu
{
 buffering_setup(1);/* Bêdzie pamietaæ w bitmapie zawartosc ekranu */
 shell_setup("VERY SIMPLE TEST",argc,argv);/* Przygotowanie okna  */

 init_plot(size,size,16,16);/* Otwarcie okna 16x16 pixeli + 16x16 znakow za pikselami*/

 /* Teraz mo¿na rysowaæ i pisac w oknie */
 for(int i=0;i<size;i++)
   for(int j=0;j<size;j++)
	{
    unsigned char c=i*size+j;//Znak "bez znaku" 
	plot(j,i,c); //Pojedynczy piksel w kolorze indeksowanym c (0..255)
	print(size+j*char_width('W'), //Wspó³rzedne w pikselach, ale znaki wiêksze!
          size+i*char_height('W'),
            "%c",c);//Znak jako znak
	}
 flush_plot();	/* Obraz/bitmapa gotowa */

 while(get_char()!=EOF);	/* Tu czeka i ignoruje ewentualne zdarzenia */

 close_plot();/* Zamykamy okno - po zabawie */
 printf("Do widzenia!\n");
 return 0;
}


