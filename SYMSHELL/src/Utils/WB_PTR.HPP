//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 	 Proste szablony inteligentnych wskaznikow oraz tablic dynamicznych - na wzor STL ale raczej niekompatybilne.
//*******************************************************************************************************************
// TEMPLATE CLASSES:
//	wb_sptr - scalar ptr
//	wb_ptr	- struct/class ptr
//	wb_pchar- ptr to char[]
//	wb_dynarray- dynamic array of
//	wb_dynmatrix- dynamic matrix of
// !!!:	ZawartoúÊ jest "sztefetowana" pomiedzy obiektami powyøszych typÛw -
// czyli w konstruktorze albo przypisaniu jest przenoszona a nie kopiowana.
// Wersja z poprawkami 2012 i poczπtkiem przenoszenia implementacji poza klasy
///////////////////////////////////////////////////////////////////////////////
#ifndef __WB_PTR_HPP__
#define __WB_PTR_HPP__

#include <stdarg.h>			//Bo jest konstruktor z nieznana liczba parametrow
#include <cassert>
#include <iostream>

#include "platform.hpp"
#include "wb_clone.hpp"

#ifdef __BCPLUSPLUS__ //Ma k≥opoty w postaci masy warningÛw "code has no effect"
#pragma warn -8019
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
#define WBPTRLOG( _P_ )  {std::cerr<<((void*)this)<<"->"<< _P_ <<'\n';}
#else
#define WBPTRLOG( _P_ )
#endif

namespace wbrtm { //WOJCIECH BORKOWSKI RUN TIME LIBRARY

//Szablon inteligentnego wskaznika dla typow sklarnych
template<class PointedType>
class wb_sptr
////////////////////////////
{
protected:
PointedType* ptr;

public:
explicit wb_sptr(PointedType* ini=NULL):ptr(ini){}
explicit wb_sptr(wb_sptr& nini):ptr(nini.give()){}
void dispose(); //Zwalnia zaalokowanπ pamiÍc
	~wb_sptr() { dispose(); } //DESTRUKTOR - SPECJALNIE NIE-JEST WIRTUALNY
void finalise(){ dispose(); } //Wywolanie rownowaøne destruktorowi
//Testy
operator int () const; //Do sprawdzania czy nie pusty
int OK() const; //Jawne sprawdzenie
//Jawne i niejawne przekazania
wb_sptr& operator = (PointedType* nini); //Przypisanie c-wskaünika
wb_sptr& operator = (wb_sptr& nini);//Sztafetujπce przypisanie
wb_sptr& operator = (const wb_sptr& nini);//Klonujπce przypisanie
wb_sptr& transfer_from(wb_sptr& nini); //Jawnie nazwany operator sztafetujπcego przypisania
void take(PointedType* nini); //Wersja jawnego przekazania mu wskaznika
PointedType*  give();//Zabiera obszar (wskazywany adres) spod zarzadu inteligentnego wskaznika!!!
//DostÍp
PointedType* get_ptr_val() const;//DostÍp wartoúci do c-wskaünika - trzeba uwaøaÊ
PointedType& operator* () const; //DostÍp do wskazywanej zawartoúci
};

//... i szablon dla typow strukturalnych
template<class PointedType>
class wb_ptr:public wb_sptr<PointedType>
/////////////////////////////////////////
{
public:
explicit wb_ptr(PointedType* ini=NULL):wb_sptr<PointedType>(ini){}
explicit wb_ptr(wb_ptr& nini); //Konstruktor transferujπcy uøywa :wb_sptr<PointedType>(nini)
		 //destruktor niepotrzebny bo takijak dziedziczony
//Przykrycie tych metod ktÛre zwracajπ wb_sptr<>, øeby zwraca≥y wb_ptr
wb_ptr& operator = (PointedType* nini); //Przypisanie c-wskaünika odpowiedniego typu
wb_ptr& operator = (wb_ptr& nini); //Przypisanie sztafetujπce (nie sptr bo wtedy nieuprawnione rozszserzenie operacji)
wb_ptr& operator = (const wb_ptr& nini);//Analogiczne klonujπce przypisanie
wb_ptr& transfer_from(wb_ptr& nini); //Jawnie nazwany operator przypisania, przykrywa

PointedType* operator -> () const;//Dodany operator -> dla wskaznika do klasy/struktury

//Jeszcze jedno jawne przekazywanie wskaünika, tyle øe zwracajπce wartoúÊ. Moøe gdzieú uøywane
wb_ptr& set(PointedType* nini) { this->take(nini);return *this;};
#pragma obsolete set

//Konstruktor klonujacy nie ma zastosowania do klas abstrakcyjnych (What???)
//wb_ptr(const wb_ptr<T>& nini):wb_sptr<T>(clone(nini.get_ptr_val()))
//	{ WBPTRLOG( "wb_ptr::CLONE CONSTRUCTOR :"<<nini.ptr )       }
};


//Klasa z rodziny wb_wskaznikÛw przeznaczona na uchwyt do lancuchow tekstowych
//Nie przechowuje dlugosci lancucha wiec tez nie sprawdza.
//(dlatego dziedziczy po wb_sptr, a nie wb_dynarray
//Z char* const char* kopiuje oczekujac ze koncza sie '\0' .
//Pomiedzy obiektami wb_pchar bez atrybutu const zawartosc jest
//jak zwykle "sztafetowana"!!!
class wb_pchar:public wb_sptr<char>
///////////////////////////////////
{
public:
explicit wb_pchar(size_t s=0):wb_sptr<char>(NULL)
	{ if(s>0 && (ptr=new char[s])!=NULL ) *ptr='\0'; }

~wb_pchar()
	{ if(ptr) delete [] ptr; ptr=NULL;/*dla sprawdzeÒ*/}

explicit wb_pchar(const char* ini);  //Kopiuje tylko dane

//wb_pchar(char* ini): wb_sptr<char>(ini) //Dwuznacznosc semantyczna, wymusza jawne decyzje
//		{}//Przypisanie danych

wb_pchar(const wb_pchar& nini);//Konstruktor klonujπcy

wb_pchar(wb_pchar& nini);//Konstruktor sztafetujπcy

wb_pchar& operator = (const char* nini); //Operator klonujπcego przypisania

wb_pchar& operator = (wb_pchar& nini); //Operator transferujπcego przypisania

wb_pchar& operator = (const wb_pchar& nini); //Operator klonujπcego przypisania

void alloc(size_t s);// Alokacja do nowego rozmiaru, utrata starej zawartoúci!

char& operator [] (size_t index) const; //Pozwala (?) na zmiana zawartosci ale nie wskaznika

size_t get_size() const; //D≥ugoúÊ ≥aÒcucha w wersji C

const char* get() const; //SkrÛcone get_ptr_val();
						//Tu nie sprawdzamy czy NULL - niech sie martwi wolajacy

//A la C-owe stringi - to co moøna bezboleúnie
wb_pchar& prn(const char* format,...); //Wyprowadza na zawartosc. Nie sprawdza rozmiaru!!!

wb_pchar& add(const char* format,...); //Dopisuje do zawartosci. Nie sprawdza rozmiaru!!!

friend size_t strlen(const wb_pchar& what);//Atrapa c-owego strlen

friend size_t strcmp(const wb_pchar& f,const wb_pchar& s);//Atrapa c-owego

friend size_t strcmp(const char* f,const wb_pchar& s); //Wersja mieszana

friend size_t strcmp(const wb_pchar& f,const char* s); //j.w.

friend const char* strchr(const wb_pchar& what,const char c);//Atrapa c-owego

friend const char* strstr(const wb_pchar& what,const wb_pchar& s);//Atrapa c-owego

friend const char* strstr(const wb_pchar& what,const char* s);//Atrapa c-owego

//Zamienia wszystkie lancuchy forrep zawarte w obiekcie wb_pchar na lancuchy whatins - bufor jest sztafetowany!!!
friend bool replace(wb_pchar& bufor,const char* forrep,const char* whatins,bool fullwords,unsigned startpos=0);//case sensitive bo brak funkcji w rodzaju stristr

//Wstawia lancuch do bufora na okreslonej pozycji - bufor jest sztafetowany(?) TODO!!!
friend bool insert(wb_pchar& bufor,unsigned pos,const char* whatins);

//Implementacja zapisu na strumien ze sprawdzaniem czy trzeba w \"
static void write(ostream& s,const char* p,char enclos='\"');
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
															   //assert(size!=0);
		if(size>0)	ptr=new T[s];
			else ptr=NULL;
		}

	//konstruktor transferujacy. Niestety uniemozliwia niektore konstrukcje, a jakos "zaslania" ten z "const wb_dynarray&"
	//wb_dynarray(wb_dynarray& nini):size(nini.size),ptr(nini.ptr)
	//	{
	//	WBPTRLOG( "wb_dynarray::TRANSFER CONSTRUCTOR("<<((void*)&nini)<<")" )
	//	nini.ptr=NULL;
	//	nini.size=0;
	//	}

	//Konstruktor "kopiujacy" - faktycznie i tak TRANSFERUJACY - i tak jest niebezpieczny
	wb_dynarray(const wb_dynarray& nini/*,bool copy=false*/):size(nini.size),ptr(nini.ptr)
		{
		WBPTRLOG( "wb_dynarray::COPY CONSTRUCTOR("<<((void*)&nini)<<")" )
																//assert(size!=0); Moøe siÍ zdarzyÊ, øe kopiujemy pustπ!
		const_cast<wb_dynarray<T>&>(nini).ptr=NULL;
		const_cast<wb_dynarray<T>&>(nini).size=0;
		}


	//konstruktor wieloparametrowy inicjujacy itemy
	explicit wb_dynarray(size_t s,T /*first,second,...*/...):size(s)
		{
		WBPTRLOG( "wb_dynarray::CONSTRUCTOR("<<size<<",T ...)" )
																assert(size>=1);
		ptr=new T[s];                                       	assert(ptr!=NULL/*After allocation*/);

		va_list list;
		va_start(list,s);
		for(size_t i=0;i<s;i++)
			ptr[i]=va_arg(list,T);
		va_end(list);
		}
	/* //TAKI NIESTETY WCHODZI W KONFLIKAT Z TYM wb_dynarray(size,...),
	   //wiÍc nie moøna go wprowadziÊ :-( bo ten poprzedni jest czÍsto uøywany w wielu miejscach
	explicit  wb_dynarray(size_t s,const T& FillVal):size(s)
		{
		WBPTRLOG( "wb_dynarray::CONSTRUCTOR("<<size<<','<<FillVal<<')' )
															   assert(size!=0);
		ptr=new T[s];
		fill(FillVal);
		}
	*/
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
size_t alloc(size_t s)
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

//SkrÛcenie wektora - przydatne przy czytaniu
//ale niebezpieczne(memcpy) lub kosztowne(for) jak T ma konstruktory i destruktory
size_t trunc(size_t s)
	{
	WBPTRLOG( "wb_dynnarray::trunc("<<s<<")" )
	if(ptr==NULL) //Bo wtedy bzdura
			return 0;
	assert(s>0); //Jak ma byc 0 to "dispose"!
	assert(s<size); //Tylko realne skracanie jest bezpieczne
	T* tmp=new T[s];
//	memcpy(tmp,ptr,s*sizeof(T));
	for(unsigned i=0;i<s;i++)
		tmp[i]=ptr[i]; //Dla klas uzyty operator przypisania
	dispose();  //Kasacja starego wektora - a destruktory?
//	delete (void*)ptr; //?Zwalnianie bez wywo≥ywania moøliwych destruktorÛw
	ptr=tmp;    //Zapamietanie nowego
	size=s;
	return size;
	}

//Rozszerzenie wektora - przydatne do rozbudowy tablicy
//ale niebezpieczne(memcpy) lub kosztowne(for) jak T ma konstruktory i destruktory
size_t expand(size_t s,const T& FillVal)
	{
	WBPTRLOG( "wb_dynnarray::expand("<<s<<")" )
	if(ptr==NULL) //Bo wtedy bzdura
			return 0;
	assert(s>size); //PowiÍkszamy!
	T* tmp=new T[s];

//	memcpy(tmp,ptr,s*sizeof(T));
	//Bezpieczne choÊ nieefektywny przepisanie zawartoúci komÛrek
	for(unsigned i=0;i<size;i++)
		tmp[i]=ptr[i]; //Dla klas uzyty operator przypisania
	//Jakieú wype≥nienie nowych komÛrek
	for(unsigned i=size;i<s;i++)
		tmp[i]=FillVal;//Dla klas uzyty operator przypisania

//	delete (void*)ptr; //?Zwalnianie bez wywo≥ywania moøliwych destruktorÛw
	dispose();  //Kasacja starego wektora - a destruktory?

	//Zapamietanie nowego
	ptr=tmp;
	size=s;
	return size;
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
#ifndef _NDEBUG
	if(ptr==NULL || index>=size)
	{ //Tu postaw breakpoint!
	 //	cerr<<"Invalid use of wb_dynarray "<<hex<<unsigned(this)<<" tabptr:"
	  //		<<hex<<unsigned(ptr)<<"index:"<<index<<" size:"<<size<<endl;
		assert(ptr!=NULL);
		assert(index<size);
	}
#endif
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

//Wymusza umieszczenie dynamicznie alokowanego
//wektora jako tablicy wewnatrz dynarray
//tylko nie zwraca this
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

//Przemieszczenie pojedynczego elementu z pozycji 0 na koniec
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
//Kontrole zakresow itp assercje dziedziczy po klasie macieøystej
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

//dealokacja wektora wektorÛw
void dispose()
	{
	WBPTRLOG( "wb_dynmatrix::dispose :"<<get_size() )
	wb_dynarray< wb_dynarray<T> >::dispose();
	}

//Rozbudowana alokacja zstepujaca
size_t alloc(size_t y,size_t x);
//Wype≥nianie
void fill(const T& Val);

};

//Implementacja niektÛrych wiÍkszych metod
//////////////////////////////////////////////////
template<class T>
inline size_t wb_dynmatrix<T>::alloc(size_t y,size_t x)
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

template<class T>
inline void wb_dynmatrix<T>::fill(const T& Val)
    {
    size_t i,H=get_size();
    for(i=0;i<H;i++)//Po wierszach
        {
        size_t L=(*this)[i].get_size();
        for(size_t j=0;j<L;j++)//Po elementach wiersza
            (*this)[i][j]=Val;//Wypelnij ten element
        }
    }

//Deklaracje I/O dla inteligentnych wskaznikow
//implementacja musi byc w innym pliku, i to raczej cpp niø hpp
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


//IMPLEMENTACJE METOD
////////////////////////////////////////////////////////////////////////////////
template<class PointedType> inline //wb_sptr::
void wb_sptr<PointedType>::dispose()
	{
	if(ptr)
		{																		WBPTRLOG( "wb_sptr::dispose :"<<ptr )
		delete ptr;
		}
	ptr=NULL;
	}

template<class PointedType> inline //wb_sptr::
wb_sptr<PointedType>&  wb_sptr<PointedType>::operator = (PointedType* nini)
	{
	this->dispose();
	ptr=nini;
	return *this;
	}

template<class PointedType> inline //wb_sptr::
void  wb_sptr<PointedType>::take(PointedType* nini) //Wersja jawnego oddania wskaznika
	{
	dispose();
	ptr=nini;
	}
template<class PointedType> inline //wb_sptr::
wb_sptr<PointedType>& wb_sptr<PointedType>::operator = (const wb_sptr<PointedType>& nini)//Klonujπce przypisanie
	{
	}

template<class PointedType> inline //wb_sptr::
wb_sptr<PointedType>&  wb_sptr<PointedType>::operator = (wb_sptr<PointedType>& nini)
	{
	dispose();
	ptr=nini.give();
	return *this;
	}

template<class PointedType> inline //wb_sptr::
wb_sptr<PointedType>&  wb_sptr<PointedType>::transfer_from(wb_sptr<PointedType>& nini) //Jawnie nazwany operator przypisania
	{
	dispose();
	ptr=nini.give();
	return *this;
	}

template<class PointedType> inline //wb_sptr::
wb_sptr<PointedType>::operator int () const
	{
	return ptr!=NULL;
	}

template<class PointedType> inline //wb_sptr::
PointedType& wb_sptr<PointedType>::operator* () const
	{																			assert(ptr!=NULL);
	return *ptr;
	}

template<class PointedType> //wb_sptr::
int inline wb_sptr<PointedType>::OK() const
	{
	return ptr!=NULL;
	}

template<class PointedType> inline//wb_sptr::
PointedType*  wb_sptr<PointedType>::get_ptr_val() const
	{					//Tu nie sprawdzamy czy NULL - niech sie martwi wolajacy
	return ptr;
	}

template<class PointedType> inline//wb_sptr::
PointedType*  wb_sptr<PointedType>::give()  //Zabiera adres zpod zarzadu inteligentnego wskaznika!!!
	{
	PointedType* pom=ptr;
	ptr=NULL;
	return pom;
	}

template<class PointedType> inline
wb_ptr<PointedType>::wb_ptr(wb_ptr<PointedType>& nini):wb_sptr<PointedType>(nini)
	{																			WBPTRLOG( "wb_ptr::TRANSFER CONSTRUCTOR used for:"<<ptr )
	}

template<class PointedType> inline
wb_ptr<PointedType>& wb_ptr<PointedType>::operator = (PointedType* nini) //Przypisanie wskaünika odpowiedniego typu
	{
	return set(nini);
	}

//Nie mozna stosowac z klasami abstrakcyjnymi (?)
template<class PointedType> inline
wb_ptr<PointedType>& wb_ptr<PointedType>::operator = (const wb_ptr<PointedType>& nini) //nie sptr bo wtedy nieuprawnione rozszserzenie operacji
	{																			WBPTRLOG( "wb_ptr::CLONE oper = :"<<nini.ptr<<"->"<<ptr )
	dispose();
	ptr=clone( nini.get_ptr_val() );
	return *this;
	}

template<class PointedType> inline
wb_ptr<PointedType>& wb_ptr<PointedType>::operator = (wb_ptr<PointedType>& nini)//Przypisanie sztafetujπce (nie sptr bo wtedy nieuprawnione rozszserzenie operacji)
	{																			WBPTRLOG( "wb_ptr::TRANSFER oper = :"<<nini.ptr<<"->"<<ptr )
	dispose();
	this->ptr=nini.give();
	return *this;
	}

template<class PointedType> inline
wb_ptr<PointedType>& wb_ptr<PointedType>::transfer_from(wb_ptr<PointedType>& nini) //Jawnie nazwany operator przypisania, przykrywa
	{
	dispose();
	this->ptr=nini.give();
	return *this;
	}

//Dodadany operator -> dla wskaznika do klasy/struktury
template<class PointedType> inline
PointedType* wb_ptr<PointedType>::operator -> () const
	{																			assert(this->ptr!=NULL);
	return this->ptr;
	}

wb_pchar::wb_pchar(const char* ini):wb_sptr<char>(NULL)  //Tylko dane
	{
	ptr=clone_str(ini);//Kopiowanie danych
	}

//PONIZSZY:Dwuznacznosc semantyczna. Konieczne w zasadzie do alokacji
//Ale to mozna zrobic konstruktorem z integerem
//wb_pchar::wb_pchar(char* ini):
//			wb_sptr<char>(ini)
//		{}//Przypisanie danych

wb_pchar::wb_pchar(const wb_pchar& nini):wb_sptr<char>(NULL)
	{																			WBPTRLOG( "wb_pchar::COPY CONSTRUCTOR :"<<(nini.ptr?nini.ptr:"@") )
	ptr=clone_str(nini.get_ptr_val());
	}//Kopiowanie danych

wb_pchar::wb_pchar(wb_pchar& nini):wb_sptr<char>(nini)
	{																			WBPTRLOG( "wb_pchar::TRANSFER CONSTRUCTOR :"<<(ptr?ptr:"@") )
																				assert(nini.ptr==NULL);//Inicjator traci dane. Jak nie to znaczy ze zaszlo niezamierzone kopiowanie
	}

wb_pchar& wb_pchar::operator = (const char* nini)
	{																			WBPTRLOG( "wb_pchar::oper = (const char*) :"<<(nini?nini:"@")<<"->"<<(ptr?ptr:"@") )
	dispose();
	ptr=clone_str(nini);
	return *this;
	}

wb_pchar& wb_pchar::operator = (wb_pchar& nini)
	{																			WBPTRLOG( "wb_pchar::TRANSFER oper = :"<<(nini.ptr?nini.ptr:"@")<<"->"<<(ptr?ptr:"@") )
	dispose();
	ptr=nini.give();
	return *this;
	}

wb_pchar& wb_pchar::operator = (const wb_pchar& nini)
	{																			WBPTRLOG( "wb_pchar::CLONE oper = :"<<(nini.ptr?nini.ptr:"@")<<"->"<<(ptr?ptr:"@") )
	dispose();
	ptr=clone_str(nini.get_ptr_val());
	return *this;
	}

void wb_pchar::alloc(size_t s)
	{																			assert(s>0);
	dispose();
	ptr=new char[s];															assert(ptr!=NULL);
	*ptr='\0';
	}

char& wb_pchar::operator [] (size_t index) const //Pozwala na zmiana zawartosci ale nie wskaznika
	{																			assert(ptr!=NULL);
	return ptr[index];
	}

size_t wb_pchar::get_size() const
	{
	return ptr?(::strlen(ptr)):(0);
	}

const char* wb_pchar::get() const
	{	//Tu nie sprawdzamy czy NULL - niech sie martwi wolajacy
	return get_ptr_val();
	}


//"ZAPRZYJAèNIONE" FUNKCJE POMOCNICZE
/////////////////////////////////////
//Operacje ≥aÒscuchowe, wypelnianie tablic i macierzy
//////////////////////////////////////////////////////////////

inline //friend
size_t strlen(const wb_pchar& what)
	{
		return ::strlen(what.get());
	}

inline //friend
size_t strcmp(const wb_pchar& f,const wb_pchar& s)
	{
		return ::strcmp(f.get(),s.get());
	}

inline //friend
size_t strcmp(const char* f,const wb_pchar& s)
	{
		return ::strcmp(f,s.get());
	}

inline //friend
size_t strcmp(const wb_pchar& f,const char* s)
	{
		return ::strcmp(f.get(),s);
	}

inline //friend
const char* strchr(const wb_pchar& what,const char c)
{
	return ::strchr(what.get(),c);
}

inline //friend
const char* strstr(const wb_pchar& what,const char* s)
{
	return ::strstr(what.get(),s);
}


template<class T> //Friend?
inline void fill(wb_dynmatrix<T>& Mat,const T& Val)
{
	Mat.fill(Val);
}

template<class T> //Friend?
inline void fill(wb_dynarray<T>& Tab,const T& Val)
{
	Tab.fill(Val);
}


} //namespace  wb_rtm

/********************************************************************/
/*			          WBRTM  version 2012                           */
/********************************************************************/
/*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
/*            W O J C I E C H   B O R K O W S K I                   */
/*    Instytut Studiow Spolecznych Uniwersytetu Warszawskiego       */
/*        WWW:  http://borkowski.iss.uw.edu.pl/                     */
/*                                                                  */
/*                               (Don't change or remove this note) */
/********************************************************************/
#endif
