/* SYMMSHELL - WB SIMPLE PORTABLE GRAPHICS & INPUT INTERFACE for C/C++  */
/************************************************************************/

#ifndef _SYMSHELL_H_
#define _SYMSHELL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* TYPES */
typedef struct ssh_point {int x,y;} ssh_point;

/* CONFIGURATION AND MENAGE SHELL - USE IN FOLLOWED ORDER! */
int  mouse_activity(int yes);			/* Ustala czy mysz ma byc obslugiwana. Zwraca poprzedni stan flagi */
void set_background(unsigned char c);	/* Ustala index koloru do czyszczenia itp */
void buffering_setup(int Yes);			/* Przelaczanie buforowanie okna */

void shell_setup(char* title,int iargc,char* iargv[]);/* Przekazanie parametrow wywolania */
int  init_plot(int a,int b,int ca, int cb); 		  /* inicjacja grafiki/semigrafiki */
void flush_plot();						/* uzgodnienie zawartosci ekranu  */
void close_plot(); 						/* zamkniecie grafiki/semigrafiki */

/* GETTING SCREEN PARAMETERS */
int  screen_height();  		/* Ostateczne rozmiary okna po przeliczeniach z init_plot*/
int  screen_width();   		/*  ...do pozycjonowania na marginesach */
int  char_height(char znak);/* Aktualne rozmiary znaku  */
int  char_width(char znak); /* ...potrzebne do pozycjonowania tekstu */
int  string_height(const char* str);/* Aktualne rozmiary lancucha */
int  string_width(const char* str); /* ...potrzebne do jego pozycjonowania */
unsigned get_background();	/* Aktualny kolor tla... */
unsigned get_buffering ();  /* Zwraca 1 jesli buforowane */

/* GETTING INPUT */
int  input_ready(); 	/* Funkcja sprawdzajaca czy jest wejscie */
int  get_char();    	/* Funkcja odczytywania znakow sterowania */
int  set_char(int);		/* Odeslanie znaku na wejscie - zwraca 0 jesli nie ma miejsca */
int  get_mouse_event(int* xpos,int* ypos,int* click);/* Funkcja odczytujaca ostatnie zdazenie myszy */
int  repaint_area(int* x,int* y,int* width,int* height);/* Podaje obszar ktory ma byc odnowiony i zwraca 0 */
	/* Jesli zwraca -1 to brak danych lub brak implementacji ! Odrysowac calosc. */
	/* Jesli zwraca -2 to znaczy ze dane juz zostaly odczytane. Nalezy zignorowac. */
/* PRINTING */
void print(int x,int y,char* format,...); /* Drukuje w kolorach domyslnych*/
void printc(int x,int y,
	unsigned char fore,unsigned char back,
	char* format,...);					  /* Drukuje w kolorach uzytkownika*/

/* DRAWING  */
void plot(int x,int y,unsigned  char c);	/* Wyswietlenie punktu na ekranie */
void line(int x1,int y1,int x2,int y2,
				unsigned  char c);			/* Wyswietlenie lini w kolorze c */
void circle(int x,int y,int r,
				unsigned char c);			/* Wyswietlenie okregu w kolorze c */
void fill_circle(int x,int y,int r,
				unsigned char c);			/* Wyswietlenie kola w kolorze c */
void fill_rect(int x1,int y1,int x2,int y2,
			    unsigned  char c);			/* Wyswietla prostokat w kolorze c */
void fill_poly(int vx,int vy,
					const ssh_point points[],int number,
					unsigned  char c);		/* Wypelnia wielokat przesuniety o vx,vy w kolorze c */

void clear_screen();						/* Czysci ekran lub ekran wirtualny */
void set_rgb(int color,int r,int g,int b);	/* Zmienia definicja koloru. Indeksy 0..255 */

/* For close_plot() */
extern int WB_error_enter_before_clean/* =0 */;

#ifdef __cplusplus
};
#endif


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



