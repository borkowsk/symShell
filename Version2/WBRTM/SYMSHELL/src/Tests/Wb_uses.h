/* "SMART" HEADER FOR SUPPORT MULTIPLATFORM COMPATYBILITY version 0.01	*/
/************************************************************************/

/* CHECK PLATFORM */
#include "../../INCLUDE/platform.h"

/* 	FORCE ANY HEADERS	       */
#define USES_STDDEF	    /* NULL, size_t, ptr_diff_t == ALWAYS! */
#define USES_LIMITS         /* A L W A Y S ! */

/*     STANDARD C HEADERS 	*/
#ifdef USES_UNISTD

#ifdef unix
#include <unistd.h>
#endif

#endif

#ifdef USES_LIMITS

#include <limits.h>
#include <float.h>

#	if !defined( __MSVC__ )
#	include <values.h>
#	else
#	define MAXDOUBLE	DBL_MAX
#	define MAXFLOAT		FLT_MAX
#	endif

#	ifdef unix
#	include <sys/param.h>
#	endif

#endif

#ifdef USES_TYPES
#include <sys/types.h>
#endif

#ifdef USES_STDIO
#include <stdio.h>
#endif

#ifdef USES_STDLIB
#include <stdlib.h>
#endif

#ifdef USES_STDARG
#include <stdarg.h>
#endif

#ifdef USES_STDDEF
#include <stddef.h>
#endif

#ifdef USES_MATH
#include <math.h>
#ifndef M_PI
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#endif
#endif

#ifdef USES_STRING
#include <string.h>
#endif

#ifdef USES_ALLOC
#include <alloc.h>
#include <malloc.h>
#endif

#ifdef USES_ASSERT
#include <assert.h>
#endif

#ifdef USES_CTYPE
#include <ctype.h>
#endif


#ifdef USES_DIR

#	ifndef __MSVC__

#	endif

#	ifdef __BORLANDC__
#	include <dir.h>
#	include <dirent.h>
#	include <direct.h>
#	endif

#	if defined( __DJGPP__ ) || defined( unix )
#	include <dir.h>
#	include <dirent.h>
#	include <direct.h>
#	include <sys/types.h>
#	include <sys/dir.h>
#	endif

#endif

#ifdef USES_ERRNO
#include <errno.h>
#endif

#ifdef USES_FCNTL
#include <fcntl.h>
#endif

#ifdef USES_IO

#	ifdef __TURBOC__
#include <io.h>
#	elif defined(unix)
#include <sys/fcntl.h>   
#	elif defined(__MSVC__)
#include <io.h>
#	endif

#endif

#ifdef USES_LOCALE
#include <locale.h>
#endif

#ifdef USES_MEM
#include <mem.h>
#endif

#ifdef USES_PROCESS
#ifdef unix
#include <stdio.h>
#else
#include <process.h>
#endif
#endif

#ifdef USES_LONGJUMP
#include <setjmp.h>
#endif

#ifdef USES_SIGNAL
#include <signal.h>
#endif

#ifdef USES_STAT
#include <sys/stat.h>
#endif

#ifdef USES_TIME
#include <time.h>

#ifndef unix
#include <sys/timeb.h>
#endif 

#endif

/* PC COMPUTERS RUNNING MSDOS ONLY */
#if defined( __GNUC__ ) && defined( __DJGPP__ )
#	if defined(USES_CONIO)||defined(USES_DOS)||defined(USES_BIOS)||defined(USES_PC)
#	include <pc.h>
#	include <dos.h>
#	include <conio.h>
#	include <bios.h>
#	endif
#elif defined(__BORLANDC__)
#	ifdef USES_CONIO
#	include <conio.h>
#	endif
#	ifdef USES_DOS
#	include <dos.h>
#	endif
#	ifdef USES_BIOS
#	include <bios.h>
#	endif
#	ifdef USES_FPU
#	include <float.h>
#	endif
#	ifdef USES_SHARE
#	include <share.h>
#	endif
#endif

/* HEADERS FOR GRAPHIX END OTHER EXTENDED PACKAGES */
#ifdef USES_BGI
// BORLAND MSDOS ONLY GRAPHICS LIBRARY
#include <graphics.h>
#endif

#ifdef USES_GRX
// GNU MSDOS+EXTENDER GRAPHICS LIBRARY ver 1.0
#include <grx.h>
#include "mousex.h"
#endif

#ifdef USES_GD
// FREEWARE GIF,XBM GRAPHICS LIBRARY
#include <gd.h>
#include "gdfontl.h"
#include "gdfonts.h"
#endif

#if defined( USES_SYMSHELL) || defined(USES_SYMULATION_SHELL)
#include "../symshell.h"
#endif


/*  STANDARD CLASS HEADERS      */
#ifdef USES_BCD
#include <bcd.h>
#endif

#ifdef USES_COMPLEX
#include <complex.h>
#endif

#ifdef USES_IOSTREAM
#include <iostream.h>
#endif

#ifdef USES_FSTREAM
#include <fstream.h>
#endif

#ifdef USES_STRSTREAM
#	ifndef unix
#include <strstrea.h>
#	else
#include <strstream.h>
#	endif
#endif

#ifdef USES_MANIP
#include <iomanip.h>
#endif
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/* Zaklad Systematyki i Geografii Roslin Uniwersytetu Warszawskiego */
/*  & Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://moderato.iss.uw.edu.pl/~borkowsk             */
/*        MAIL: borkowsk@iss.uw.edu.pl                              */
/*                               (Don't change or remove this note) */
/********************************************************************/



