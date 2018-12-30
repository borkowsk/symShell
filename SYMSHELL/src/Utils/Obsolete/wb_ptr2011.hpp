//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 	 Proste szablony inteligentnych wskaznikow oraz tablic dynamicznych - na wzor STL ale raczej niekompatybilne.
//*******************************************************************************************************************
// TEMPLATE CLASSES: 
//	wb_sptr - scalar ptr 
//	wb_ptr	- struct/class ptr
//	wb_pchar- ptr to char[]
//	wb_dynarray- dynamic array of
//	wb_dynmatrix- dynamic matrix of
// !!!:	Zawarto�� jest "sztefetowana" pomiedzy obiektami powy�szych typ�w -
// w konstruktorze albo przypisaniu jest przenoszona a nie kopiowana.
/////////////////////////////////////////////////////////////////////////////
#ifndef __WB_PTR_HPP__
#define __WB_PTR_HPP__

#ifndef __cplusplus
#error C++ required
#endif

#if _MSC_VER > 1000
#pragma warning( disable : 4521 4522 )	//multiple copy constructor/operator -bo osobne dla const char* i char* 
//#pragma warning( disable : 4512 )		//assignment operator could not be generated 
#endif // _MSC_VER > 1000

#ifndef HIDE_WB_PTR_IO 
#define HIDE_WB_PTR_IO 0
#endif

#ifndef DEBUG_WB_PTR
#define DEBUG_WB_PTR 0
#endif

#if DEBUG_WB_PTR == 1
#include <iostream.h>
#define WBPTRLOG( _P_ )  {cerr<<((void*)this)<<"->"<< _P_ <<'\n';}
#else
class istream;//Obecne tez w innych deklaracjach
class ostream;//ale bez implementacji
#define WBPTRLOG( _P_ )
#endif

#include <stdarg.h>			//Jest konstruktor z nieznana liczba parametrow
#include <assert.h>
#include "wb_clone.hpp"

//Szablon inteligentnego wskaznika dla typow sklarnych
template<class T>
class wb_sptr
////////////////////////////
{
protected:
T* ptr;
public:
	explicit wb_sptr(T* ini=NULL):ptr(ini){}
	wb_sptr(wb_sptr& nini):ptr(nini.give()){}		
	~wb_sptr(){ dispose(); }

void finalise(){ dispose(); }//Wywolanie rownowa�ne destruktorowi

void dispose()
	{ 	
	if(ptr) 
		{
		WBPTRLOG( "wb_sptr::dispose :"<<ptr )
		delete ptr;
		}
	ptr=NULL;
	}

wb_sptr& operator = (T* nini) 
	{ 
	dispose(); 
	ptr=nini;
	return *this;
	}

void take(T* nini) //Wersja jawnego oddania wskaznika
	{ 
	dispose(); 
	ptr=nini;
	}

wb_sptr<T>& operator = (wb_sptr<T>& nini) 
	{ 
	dispose(); 
	ptr=nini.give();
	return *this;
	}

wb_sptr<T>& transfer_from(wb_sptr<T>& nini) //Jawnie nazwany operator przypisania
	{ 
	dispose(); 
	ptr=nini.give();
	return *this;
	}

operator int () const
	{
	return ptr!=NULL;
	}

T& operator* () const
	{
	assert(ptr!=NULL);
	return *ptr;
	}

int OK() const
	{
	return ptr!=NULL;
	}

T* get_ptr_val() const
	{
	//Tu nie sprawdzamy czy NULL - niech sie martwi wolajacy
	return ptr;
	}

//Zabiera adres zpod zarzadu inteligentnego wskaznika!!!
T*  give()
	{
	T* pom=ptr;
	ptr=NULL;
	return pom;
	}

};


//... i szablon dla typow strukturalnych
template<class T>
class wb_ptr:public wb_sptr<T>
/////////////////////////////////////////
{
public:
	explicit wb_ptr(T* ini=NULL):wb_sptr<T>(ini)
	{}

	wb_ptr(wb_ptr& nini):wb_sptr<T>(nini)
	{
		WBPTRLOG( "wb_ptr::TRANSFER CONSTRUCTOR :"<<ptr )
	}

//Konstruktor klonujacy nie ma zastosowania do klas abstrakcyjnych
//wb_ptr(const wb_ptr<T>& nini):wb_sptr<T>(clone(nini.get_ptr_val()))		
//	{
//		WBPTRLOG( "wb_ptr::CLONE CONSTRUCTOR :"<<nini.ptr )
//	}

wb_ptr& set(T* nini)
	{
	dispose(); 
	ptr=nini;
	return *this;
	}

wb_ptr& operator = (T* nini) 
	{ 	
	return set(nini);
	}

//Nie mozna stosowac z klasami abstrakcyjnym
//wb_ptr& operator = (const wb_ptr<T>& nini) //nie sptr bo wtedy nieuprawnione rozszserzenie operacji
//	{ 
//	WBPTRLOG( "wb_ptr::CLONE oper = :"<<nini.ptr<<"->"<<ptr )
//	dispose(); 
//	ptr=clone(nini.get_ptr_val());
//	return *this; 
//	}

wb_ptr& operator = (wb_ptr<T>& nini) //nie sptr bo wtedy nieuprawnione rozszserzenie operacji
	{ 
	WBPTRLOG( "wb_ptr::TRANSFER oper = :"<<nini.ptr<<"->"<<ptr )
	dispose(); 
	ptr=nini.give();
	return *this; 
	}

wb_ptr& transfer_from(wb_ptr<T>& nini) //Jawnie nazwany operator przypisania
	{ 
	dispose(); 
	ptr=nini.give();
	return *this;
	}

//Dodadany operator -> dla wskaznika do klasy/struktury
T* operator -> () const
	{
	assert(ptr!=NULL);
	return ptr;
	}
};

//Klasa z rodziny przeznaczona na uchwyt do lancuchow tekstowych
//Nie przechowuje dlugosci lancucha wiec tez nie sprawdza.
//Z char* const char* kopiuje oczekujec ze koncza sie '\0' .
//Pomiedzy obiektami wb_pchar bez atrybutu const zawartosc jest
//jak zwykle "sztafetowana"!!!
class wb_pchar:public wb_sptr<char>
///////////////////////////////////
{
public:
	explicit wb_pchar(size_t s=0):wb_sptr<char>(NULL)
	{ 
	if(s>0) ptr=new char[s];
	if(ptr!=NULL) *ptr='\0';
	}

	explicit wb_pchar(const char* ini):wb_sptr<char>(NULL)
	{
	ptr=clone_str(ini);//Kopiowanie danych
	}

//PONIZSZY:Dwuznacznosc semantyczna. Konieczne w zasadzie do alokacji
//Ale to mozna zrobic konstruktorem z integerem
//wb_pchar(char* ini):	
//			wb_sptr<char>(ini)
//		{}//Przypisanie danych

	wb_pchar(const wb_pchar& nini):wb_sptr<char>(NULL)
	{
	WBPTRLOG( "wb_pchar::COPY CONSTRUCTOR :"<<(nini.ptr?nini.ptr:"@") )
	ptr=clone_str(nini.get_ptr_val());
	}//Kopiowanie danych

	wb_pchar(wb_pchar& nini):wb_sptr<char>(nini)
	{
	WBPTRLOG( "wb_pchar::TRANSFER CONSTRUCTOR :"<<(ptr?ptr:"@") )
	assert(nini.ptr==NULL);//Inicjator traci dane. Jak nie to znaczy ze zaszlo niezamierzone kopiowanie
	} 

wb_pchar& operator = (const char* nini) 
	{ 
	WBPTRLOG( "wb_pchar::oper = (const char*) :"<<(nini?nini:"@")<<"->"<<(ptr?ptr:"@") )
	dispose(); 
	ptr=clone_str(nini);
	return *this;
	}

wb_pchar& operator = (wb_pchar& nini) 
	{ 
	WBPTRLOG( "wb_pchar::TRANSFER oper = :"<<(nini.ptr?nini.ptr:"@")<<"->"<<(ptr?ptr:"@") )
	dispose(); 
	ptr=nini.give();
	return *this;
	}

wb_pchar& operator = (const wb_pchar& nini) 
	{ 
	WBPTRLOG( "wb_pchar::CLONE oper = :"<<(nini.ptr?nini.ptr:"@")<<"->"<<(ptr?ptr:"@") )
	dispose(); 
	ptr=clone_str(nini.get_ptr_val());
	return *this;
	}

void alloc(size_t s)
	{
	assert(s>0);
	dispose();
	ptr=new char[s];
	assert(ptr!=NULL);
	*ptr='\0';
	}

char& operator [] (size_t index) const //Pozwala na zmiana zawartosci ale nie wskaznika
	{
	assert(ptr!=NULL);
	return ptr[index];
	}

size_t get_size() const
	{ 
	return ptr?strlen(ptr):0;
	}

const char* get() const
	{
	//Tu nie sprawdzamy czy NULL - niech sie martwi wolajacy
	return get_ptr_val();
	}

//Wyprowadza na zawartosc. Nie sprawdza rozmiaru!!!
wb_pchar& prn(const char* format,...);

//Dopisuje do zawartosci. Nie sprawdza rozmiaru!!!
wb_pchar& cat(const char* format,...);

//Implementacja zapisu na strumien ze sprawdzaniem czy trzeba w \"
static 
void write(ostream& s,const char* p,char enclos='\"');

//Zamienia fragment lancucha forrep zawartego w obiekcie wb_pchar  na lancuch whatins
friend
bool replace(wb_pchar& bufor,const char* forrep,const char* whatins,bool fullwords);//case sensitive bo brak funkcji w rodzaju stristr

friend
size_t strlen(const wb_pchar& what)
	{
		return ::strlen(what.get());
	}

friend
size_t strcmp(const wb_pchar& f,const wb_pchar& s)
	{
		return ::strcmp(f.get(),s.get());
	}

friend
size_t strcmp(const char* f,const wb_pchar& s)
	{
		return ::strcmp(f,s.get());
	}

friend
size_t strcmp(const wb_pchar& f,const char* s)
	{
		return ::strcmp(f.get(),s);
	}

friend
const char* strchr(const wb_pchar& what,const char c)
	{
		return ::strchr(what.get(),c);
	}

};

//Szablon bardzo prostej tablicy o rozmiarze dynamicznym 
//i mozliwym testowaniu indeksow (assercja!!!) przy odwolaniach
template<class T>
class wb_dynarray
/////////////////
{
T* ptr;		//wskaznik do wewnetrznego wektora
size_t size;//rozmiar aktualnie zaalokowanego wektora
public:
	//konstruktor alokujacy
	explicit wb_dynarray(size_t s=0):size(s)
		{ 
		WBPTRLOG( "wb_dynarray::CONSTRUCTOR("<<size<<')' )		
		if(s>0)	ptr=new T[s];
			else ptr=NULL;
		}

	//konstruktor transferujacy. Niestety uniemozliwia niektore konstrukcje, a jakos "zaslania" ten z "const wb_dynarray&"
	//wb_dynarray(wb_dynarray& nini):size(nini.size),ptr(nini.ptr)
	//	{
	//	WBPTRLOG( "wb_dynarray::TRANSFER CONSTRUCTOR("<<((void*)&nini)<<")" )		
	//	nini.ptr=NULL;
	//	nini.size=0;
	//	}
	
	//Konstruktor "kopiujacy" - musi byc forsowany wiec jest niebezpieczny
	wb_dynarray(const wb_dynarray& nini/*,bool copy=false*/):size(nini.size),ptr(nini.ptr)
		{
		WBPTRLOG( "wb_dynarray::COPY CONSTRUCTOR("<<((void*)&nini)<<")" )		
		const_cast<wb_dynarray<T>&>(nini).ptr=NULL;
		const_cast<wb_dynarray<T>&>(nini).size=0;
		}
	

	//konstruktor wieloparametrowy inicjujacy itemy
	explicit wb_dynarray(size_t s,T /*first,second,...*/...):size(s)
		{
		WBPTRLOG( "wb_dynarray::CONSTRUCTOR("<<size<<",T ...)" )		
		assert(s>=1); 
		ptr=new T[s];
		assert(ptr!=NULL/*After allocation*/);
	
		va_list list;
		va_start(list,s);
		for(size_t i=0;i<s;i++)		
			ptr[i]=va_arg(list,T);
		va_end(list);
		}

	//destruktor zwalnia pamiec!
	~wb_dynarray()
		{ 
		WBPTRLOG( "wb_dynarray::DESTRUCTOR" )		
		dispose(); 
		}

int OK() const
	{
	return ptr!=NULL;
	}

//Czy tablica zostala zalokowana
int IsOK() const
	{
		return ptr!=NULL;
	}

//Alokacja wektora
int alloc(size_t s)
	{
	WBPTRLOG( "wb_dynnarray::alloc("<<s<<")" )
	assert(s>0);
	if(get_size()>0)
		dispose();

	ptr=new T[s];
	if(ptr==NULL) 
			return 0;
	size=s;
	return s;
	}

//Dealokacja wektora
void dispose()
	{ 
	WBPTRLOG( "wb_dynarray::dispose() :"<<size )
#ifdef USES_NONSTD_ALLOCATORS	
	if(ptr) delete [size]ptr;
#else
	if(ptr) delete []ptr;	
#endif	
	ptr=NULL;
	size=0;
	}

//dostep do pojedynczego itemu
T& operator [] (size_t index) const
	{
	assert(ptr!=NULL);
	assert(index<size);
	return ptr[index];
	}

//Rozmiar wewnetrznego wektora
size_t get_size() const
	{ return size;}

//Daje bezposredni dostep do wewnetrznego wektora
//ale nie wolno go zdealokowac!!!
T* get_ptr_val(size_t offset=0,size_t interest=0) const
	{
	if(ptr==NULL ||
	   offset>=size ||
	   offset+interest>=size+1
	   )
		   return NULL;
	return ptr+offset;
	}

//Zabiera adres zpod zarzadu tablicy!!!
T*  give_dynamic_ptr_val(size_t& outsize)
	{
	T* pom=ptr;
	outsize=size;
	//kasowanie
	ptr=NULL;size=0;
	return pom;
	}

//Wymusza umieszczenie dynamicznie alokowanego 
//wektora jako tablicy wewnatrz dynarray
wb_dynarray& set_dynamic_ptr_val(T* iptr,size_t isiz)
	{
	dispose();
	ptr=iptr;
	if(ptr!=NULL)
		size=isiz;
		else
		size=0;
	return *this;
	}

void take(T* iptr,size_t isiz)
	{
	dispose();
	ptr=iptr;
	if(ptr!=NULL)
		size=isiz;
		else
		size=0;
	}

void take(wb_dynarray& nini)
	{
	if(size>0)
		dispose(); 
	ptr=nini.ptr;
	size=nini.size;
	nini.ptr=NULL;
	nini.size=0;
	}

//Transferujacy operator przypisania
wb_dynarray<T>& operator = (wb_dynarray<T>& nini) 
	{ 
	WBPTRLOG( "wb_dynnarray::operator = (wb_dynarray& "<<((void*)&nini)<<")" )
	this->take(nini);
	return *this;
	}

//Rozne wygodne funkcje pomocnicze
void fill(const T& Val)
{
	size_t i,H=get_size();
	for(i=0;i<H;i++)//Po elementach
	{
		(*this)[i]=Val;//Wypelnij ten element
	}
}

//Przesuniecie pojedynczego elementu na koniec
void shift_left(size_t index)
{
    char bufor[sizeof(T)];
    assert(ptr!=NULL);
    assert(index<size);
    if(index>=size-1) return;//Nic nie trzeba robic
    memcpy(bufor,ptr+index,sizeof(T));
    size_t ile=size-index-1;
    memcpy(ptr+index,ptr+(index+1),sizeof(T)*ile);
    memcpy(ptr+(size-1),bufor,sizeof(T));
}

void copy_from(const T Where[],size_t how_many_elements)
{
    assert(how_many_elements<=size);
    memcpy(ptr,Where,how_many_elements*sizeof(T));
}

void copy_from(const wb_dynarray<T>& Where)
{
    assert(Where.get_size()<=size);
    memcpy(ptr,Where.get_ptr_val(),Where.get_size()*sizeof(T));
}

};

//Szablon prostej tablicy dwuwymiarowej 
//o dowolnej ilosci wierszy i dowolnej dlugosci kazdego wiersza.
//Kontrole zakresow itp assercje dziedziczy po klasie macie�ystej
template<class T>
class wb_dynmatrix:public wb_dynarray< wb_dynarray<T> >
/////////////////////////////////////////////////////////////
{
public:	
	explicit wb_dynmatrix(size_t y=0,size_t x=0):wb_dynarray< wb_dynarray<T> >(y)
		{ 
		WBPTRLOG( "wb_dynmatrix::CONSTRUCTOR("<<y<<','<<x<<')' )		
		if(y>0 && x>0)
			for(size_t Y=0;Y<y;Y++)
				(*this)[Y].alloc(x);
		}
	
	explicit wb_dynmatrix(size_t s,wb_dynarray<T>* ...):wb_dynarray<wb_dynarray<T> >(s)
		{
		WBPTRLOG( "wb_dynmatrix::CONSTRUCTOR("<<get_size()<<",wb_dynarray<T>* ...)" )		
		assert(get_size()>=1); 
		
		va_list list;
		va_start(list,s);
		for(size_t i=0;i<s;i++)		
			(*this)[i]=*(va_arg(list,wb_dynarray<T>*));//Czy to wskaznik czy referencja to rybka
	
		va_end(list);
		}

	wb_dynmatrix(wb_dynmatrix& nini):
		wb_dynarray< wb_dynarray<T> >(nini)
		{
		WBPTRLOG( "wb_dynmatrix::TRANSFER CONSTRUCTOR("<<((void*)&nini)<<")" )		
		}
		
	~wb_dynmatrix()
		{ 
		WBPTRLOG( "wb_dynmatrix::DESTRUCTOR" )		
		dispose();
		}

//dealokacja wektora wektor�w
void dispose()
	{ 
	WBPTRLOG( "wb_dynmatrix::dispose :"<<get_size() )
	wb_dynarray< wb_dynarray<T> >::dispose();
	}

//Rozbudowana alokacja zstepujaca
int alloc(size_t y,size_t x)
	{
	WBPTRLOG( "wb_dynmatrix::alloc("<<y<<','<<x<<")" )
	assert(y>0);
	if(get_size()>0)
		dispose();
	
	if(wb_dynarray< wb_dynarray<T> >::alloc(y)==0)
				return 0;

	if(y>0 && x>0)
			for(size_t Y=0;Y<y;Y++)
				{
				if((*this)[Y].alloc(x)==0)
						return 0;
				}

	return get_size();
	}

void fill(const T& Val)
    {
    size_t i,H=get_size();
    for(i=0;i<H;i++)//Po wierszach
        {
        size_t L=(*this)[i].get_size();
        for(size_t j=0;j<L;j++)//Po elementach wiersza
            (*this)[i][j]=Val;//Wypelnij ten element
        }
    }

};


//FUNKCJE POMOCNICZE
/////////////////////////////////////
//Wypelnianie tablic i macierzy
//////////////////////////////////////////////////////////////
template<class T>
inline void fill(wb_dynmatrix<T>& Mat,const T& Val)
{
	Mat.fill(Val);
}

template<class T>
inline void fill(wb_dynarray<T>& Tab,const T& Val)
{
    Tab.fill(Val);
}


//I/O dla inteligentnych wskaznikow - Tylko deklaracje
//implementacja musi byc w innym pliku
/////////////////////////////////////////////////////////////////
#if	HIDE_WB_PTR_IO != 1

template<class T>
ostream& operator<<(ostream&,const wb_sptr<T>&);
template<class T>
istream& operator>>(istream&,wb_sptr<T>&);

ostream& operator<<(ostream&,const wb_sptr<char>&);
istream& operator>>(istream&,wb_sptr<char>&);
ostream& operator<<(ostream&,const wb_pchar&);
istream& operator>>(istream&,wb_pchar&);

template<class T>
ostream& operator<<(ostream&,const wb_dynarray<T>&);
template<class T>
istream& operator>>(istream&,wb_dynarray<T>&);

template<class T>
ostream& operator<<(ostream&,const wb_dynmatrix<T>&);
template<class T>
istream& operator>>(istream&,wb_dynmatrix<T>&);
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


