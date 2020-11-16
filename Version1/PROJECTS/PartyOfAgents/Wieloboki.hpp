//  Klasa wielobok i biblioteka ró¿nych wieloboków
//  Przyk³ad definiowania dosyæ prostej klasy
//        ale maj¹cej przeci¹¿one operatory [] i ()
//////////////////////////////////////////////////////////////
#include "symshell.h" //Potrzebne typy z symshell'a

//Definicja klasy Wielobok - niemal bez ¿adnej implementacji metod
class   Wielobok
//////////////////////////////////
{
private: //Pola prywatne - nikt nie ma bezpoœredniego dostêpu    
//Takie pola bêd¹ w ka¿dym obiekcie tej klasy: 
ssh_point* Punkty; //WskaŸnik do listy punktów      
unsigned   Ilobok;//"Ilobok"? Czyli ile jest punktów na liœcie  

public: //Interface - metody widoczne dla u¿ytkowników klasy

//Metody dzia³aj¹ce te¿ dla Wieloboków sta³ych (const)  
unsigned IlePunktow() const {return Ilobok;} //To mo¿e byæ "inline" - szybkie   
const ssh_point& DajPunkt(unsigned Ktory) const;//A to lepierj nie tu, bo "zaciemni" kod
        //Ta metoda zwraca "sta³¹ referencjê" do punktu, co pozwala na czytanie
        //ale nie pozwala modyfikowaæ tego punktu
        
int DajMinX() const;
int DajMaxX() const;
int DajMinY() const;
int DajMaxY() const;

const ssh_point& operator [] (unsigned Ktory) const //Wersja operatorowa!!!!
                 {return DajPunkt(Ktory);}        

//Rysuj() - rysuje gdzieœ wielobok w zadanym kolorze. Nie modyfikuje stanu!
void Rysuj(int x,int y,ssh_color Color) const;  

void operator () (int x,int y,ssh_color Color) const //Wersja operatorowa!!!!
                 { Rysuj(x,y,Color);}

//Konstruktory - tworz¹ obiekty klasy Wielobok 
//W tym wypadku tylko na bazie wzorców - tablic albo innych Wieloboków     
//Nie ma mo¿liwoœci stworzenia pustego "Wieloboku", 
//choæ na upartego mo¿e byæ "zerowy"  
Wielobok(const ssh_point Wzorek[],unsigned RozmiarWzorku);
Wielobok(const Wielobok& Wzorek); //tzw. konstruktor kopuj¹cy, którego parametrem 
               //jest odniesienie - tzw. "sta³a referencja" do obiektu tego samego
               //typu. Jego zawartoœæ musi byæ starannie przekopiowana

//Destruktor - bo trzeba zwolniæ pomocnicz¹ tablice.
//"virtual" - bo tak jest bezpieczniej.
//Co to znaczy bêdzie innym razem :-) 
virtual ~Wielobok();       

//Transformacje. Modyfikuj¹ listê punktów ¿eby by³o wygodniej
void OdbijWPionie();//Zmienia w odbicie lustrzane pionowo
void OdbijWPoziomie();//Zmienia w odbicie lustrzane poziomo
void ObrocORad(double Radiany);//Obraca o ileœ radianów
void Skaluj(double x,double y);//Zmienia wspó³rzêdne
void Centruj();//Zmienia wspó³rzêdne tak, ¿eby by³y wokó³ œrodka ciê¿koœci
     //Uwaga - trzy ostatnie s¹ "niszcz¹ce", bo punkty s¹ pamiêtane 
     //na liczbach ca³kowitych a wyniki nie musz¹ takie byæ!
              
//Biblioteka podstawowych kszta³tów w przestrzeni nazw klasy Wielobok, 
//Zrobione jako statyczne metody bo to pozwala robiæ potem ró¿ne implementacje 
//przechowywania tych wieloboków, np. tworzyæ je dopiero gdy bêd¹ potrzebne
//lub œci¹gaæ z dysku. 
//Zwracaj¹ "sta³e referencje" czyli mo¿na czytaæ, rysowaæ, ale nie zmieniaæ. 
//"static" w odniesieniu do metody oznacza, ¿e jest to sk³adowa klasy jako takiej
// a nie ka¿dego obiektu tej klasy z osobna. Do wywo³ania obiekt nie jest potrzebny!
static const Wielobok& Romb();
static const Wielobok& Domek();
static const Wielobok& Ufo();     
static const Wielobok& Ludzik(int typ); 
};


