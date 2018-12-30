#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#ifdef __GNUC__
#include <libbcc.h>
#else
#include <graphics.h>
#endif

char** M_argv;
extern void symulation();

int main(int argc,char* argv[])
{
M_argv=argv;
randomize();
	/* request autodetection */
	int gdriver = DETECT, gmode, errorcode;

	/* initialize graphics mode */
	initgraph(&gdriver, &gmode, "c:\\c3\\bgi");

	/* read result of initialization */
	errorcode = graphresult();

	if (errorcode != grOk)    /* an error occurred */
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		exit(1);               /* return with error code */
	}

	directvideo=0;

	symulation();

	return 0;
}

/* clean up */
#pragma exit closegraph 254