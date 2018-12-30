// 	 Prosty szablon inteligentnych wskaznikow oraz tablic dynamicznych
//**************************************************************************
// TEMPLATE CLASSES: wb_ptr & wb_dynarray
// -na wzor STL ale raczej niekompatybilny.


#ifndef __WB_PTR_HPP__
#define __WB_PTR_HPP__

#include <stdio.h> //bo NULL i printf
#include <stdarg.h>//Konstruktor z nieznana liczba parametrow
#include <assert.h>

template<class T>
class wb_ptr
{
T* ptr;
public:
	wb_ptr(T* ini=NULL):ptr(ini){}
	wb_ptr(wb_ptr& nini):ptr(nini.ptr)
		{nini.ptr=NULL;}

	~wb_ptr(){ dispose(); }

void dispose()
	{ 
	if(ptr) delete ptr;
	ptr=NULL;
	}

wb_ptr& operator = (T* nini) 
	{ 
	dispose(); 
	ptr=nini;
	return *this;
	}

wb_ptr& operator = (wb_ptr& nini) 
	{ 
	dispose(); 
	ptr=nini.ptr;
	nini.ptr=NULL;
	return *this;
	}

operator int ()
	{
	return ptr!=NULL;
	}

T& operator* ()
	{
	assert(ptr!=NULL);
	return *ptr;
	}

T* operator -> ()
	{
	assert(ptr!=NULL);
	return ptr;
	}

T* get_ptr_val()
	{
	assert(ptr!=NULL);
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


template<class T>
class wb_dynarray
{
T* ptr;
size_t size;
public:
	wb_dynarray(size_t s=0):size(s)
		{ 
		if(s>0) ptr=new T[s];
			else  ptr=NULL;
		}

	wb_dynarray(size_t s,T /*first,second,...*/...):size(s)
		{
		assert(s>=1); 
		ptr=new T[s];
		assert(ptr!=NULL/*After allocation*/);
	
		va_list list;
		va_start(list,s);
		for(size_t i=0;i<s;i++)		
			ptr[i]=va_arg(list,T);
		va_end(list);
		}
	
	wb_dynarray(wb_dynarray& nini):size(nini.size),ptr(nini.ptr)
		{ nini.ptr=NULL;}
	
	~wb_dynarray()
		{ dispose(); }

void dispose()
	{ 
#ifdef USES_NONSTD_ALLOCATORS	
	if(ptr) delete [size]ptr;
#else
	if(ptr) delete []ptr;	
#endif	
	ptr=NULL;
	}

void alloc(size_t s)
	{
	assert(s>0);
	dispose();
	ptr=new T[s];
	assert(ptr!=NULL);
	size=s;
	}

wb_dynarray& operator = (wb_dynarray& nini) 
	{ 
	dispose(); 
	ptr=nini.ptr;
	size=nini.size;
	nini.ptr=NULL;
	return *this;
	}

operator int ()
	{
	return ptr!=NULL;
	}

T& operator [] (size_t index)
	{
	assert(ptr!=NULL);
	assert(index<size);
	return ptr[index];
	}

size_t get_size()
	{ return size;}

void set_dynamic_ptr_val(T* iptr,size_t isiz)
	{
	dispose();
	ptr=iptr;
	if(ptr!=NULL)
		size=isiz;
		else
		size=0;
	}

T* get_ptr_val(size_t offset=0)
	{
	assert(ptr!=NULL);
	assert(offset<size);
	return ptr+offset;
	}

//Zabiera adres zpod zarzadu tablicy!!!
T*  give()
	{
	T* pom=ptr;
	ptr=NULL;
	return pom;
	}

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



