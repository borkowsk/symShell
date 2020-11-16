#include <stdlib.h>
#include <stdio.h>
#include "pseudoBGI.h" //#include <graphics.h>#include <conio.h>
using namespace BorlandCompat;

char** M_argv;
extern void symulation();

int main(int argc,char* argv[])
{
M_argv=argv;
randomize();
	/* initialize graphics mode */
	/* request autodetection */
	//int gdriver = DETECT, gmode, errorcode;
	//initgraph(&gdriver, &gmode, "c:/c/bgi ");

	initgraph(640,480,16);//Windows!!!

	/* read result of initialization */
	int errorcode = graphresult();


	if (errorcode != grOk)    /* an error occurred */
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		exit(1);               /* return with error code */
	}

	//directvideo=0;//Precambric!!!

	symulation();

	return 0;
}

/* clean up */
#pragma exit closegraph 254