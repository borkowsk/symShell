// Klasa stopera liczacego czas CPU od momentu
// utworzenia lub wywolania reset() do chwili pobrania
// wartosci za pomoca "operator double()"
//////////////////////////////////////////////////////////////////
#ifndef __CTICKER_HPP_
#define __CTICKER_HPP_

class cticker
{
clock_t begin_val;
public:
cticker()
{ begin_val=clock(); }

void reset()
{ begin_val=clock(); }

operator double()
{
	clock_t curr=clock();
return (double( curr ) - double( begin_val ))/
#ifdef __IRIX32__
	double(CLOCKS_PER_SEC);
	//double( sysconf(_SC_CLK_TCK) ); 
#else
	double( CLK_TCK ); 
#endif
}

void finalise()
{
this->~cticker();
}

#ifdef USES_IOSTREAM
friend
ostream& operator << (ostream& o, cticker c)
{ o<<(double)c<<"s ";return o; }
#endif

};

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



