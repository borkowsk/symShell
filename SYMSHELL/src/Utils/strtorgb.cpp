#include <cctype>
#include <cstdlib>
//using namespace std;
#include "INCLUDE/wb_ptr.hpp"

//Funkcja interpretuj¹ca string jako wartoœæ RGB
//Dopuszczalne formaty to: xFFFFFF  b111111111111111111111111  rgb(255,255,255) RGB(255,255,255) oraz dziesiêtny oczywiœcie

inline unsigned strtounsigned(const char *s, char **endptr)
{
   while(isspace(*s)) s++;   //isblank() Usun biale       ?
   if(s[0]=='0' && tolower(s[1])=='x' )
		return  strtoul(s,endptr,16);
   else
   if(tolower(s[0])=='x')
		return  strtoul(s+1,endptr,16);
   else
   if(tolower(s[0])=='o')
		return  strtoul(s+1,endptr,8);
   else
   if(tolower(s[0])=='b')
		return  strtoul(s+1,endptr,2);
   else
   {
		return strtoul(s,endptr,10);
   }
}

unsigned strtorgb(const char *s, char **endptr)
{
   while(isspace(*s)) s++;   //isblank() Usun biale       ?


   if(tolower(s[0])=='r' &&  tolower(s[1])=='g' && tolower(s[2])=='b' && tolower(s[3])=='(')
   {
		wbrtm::wb_pchar pom(s+4);
		char* endptr2=NULL;

		char* token=strtok(pom.get_ptr_val(),",");
		if(token==NULL) { *endptr=(char*)s;return 0; }
		unsigned val=strtounsigned(token,&endptr2);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

		token=strtok(NULL,",");
		if(token==NULL) { *endptr=(char*)s;return 0; }
		val=256*val+strtounsigned(token,&endptr2);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

		token=strtok(NULL,")");
		if(token==NULL) { *endptr=(char*)s;return 0; }
		val=256*val+strtounsigned(token,&endptr2);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

		return val;
   }
   else
   if(tolower(s[0])=='a' && tolower(s[1])=='r' &&  tolower(s[2])=='g' && tolower(s[3])=='b' && tolower(s[4])=='(')
   {
		wbrtm::wb_pchar pom(s+5);
		char* endptr2=NULL;

		char* token=strtok(pom.get_ptr_val(),",");
		if(token==NULL) { *endptr=(char*)s;return 0; }
		unsigned val=strtounsigned(token,&endptr2);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

		token=strtok(NULL,",");
		if(token==NULL) { *endptr=(char*)s;return 0; }
		val=256*val+strtounsigned(token,&endptr2);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

		token=strtok(NULL,",");
		if(token==NULL) { *endptr=(char*)s;return 0; }
		val=256*val+strtounsigned(token,&endptr2);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

		token=strtok(NULL,")");
		if(token==NULL) { *endptr=(char*)s;return 0; }
		val=256*val+strtounsigned(token,&endptr2);
		if(endptr2!=NULL && *endptr2!='\0') { *endptr=(char*)s;return 0; }

		return val;
   }
   else
   {
	   return strtounsigned(s,endptr);
   }
}
