#include <cstdlib>
//using namespace std;
#include "INCLUDE/wb_ptr.hpp"

//Funkcja interpretuj¹ca string jako wartoœæ RGB
//Dopuszczalne formaty to: xFFFFFF  b111111111111111111111111  rgb(255,255,255) RGB(255,255,255)

unsigned strtorgb(const char *s, char **endptr)
{  //char* endptr=NULL;
   while(isblank(*s)) s++;   //Usun biale

   if(s[0]=='0' && tolower(s[1])=='x' )
		return  strtoul(s,endptr,16);    // strtol
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
   if(tolower(s[0])=='r' &&  tolower(s[1])=='g' && tolower(s[2])=='b' && tolower(s[3])=='(')
   {
		wbrtm::wb_pchar pom(s+4);
		char* token=strtok(pom.get_ptr_val(),",");
		if(token==NULL) { *endptr=(char*)s;return 0; }
		token=strtok(NULL,",");
		if(token==NULL) { *endptr=(char*)s;return 0; }
		token=strtok(NULL,")");
		if(token==NULL) { *endptr=(char*)s;return 0; }

		return 1;
   }
   else
		return 0;
}
