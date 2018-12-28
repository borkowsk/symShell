SYMSHELL 0.91
(Zaprojektowa³ i zaimplementowa³ Wojciech Borkowski)
 
PRZENOŒNY FUNKCJE GRAFICZNE DLA PROSTYCH APLIKACJI C/C++
	Problem przenoœnoœci aplikacji pomiêdzy ró¿nymi platformami sprzêtowymi i systemami operacyjnymi jest zmor¹ mêcz¹c¹ programistów od zarania dziejów informatyki. Jêzyki C i C++ nie s¹ pod tym wzglêdem najgorsze bowiem istniej¹ ich ogólnie przyjête standardy pozwalaj¹ce uzyskaæ przenoœnoœæ na poziomie kodu Ÿród³owego o ile tylko istnieje kompilator C/C++ na dan¹ platformê. K³opot z tym ze standardy te nie obejmuj¹ bardziej zaawansowanych operacji wejœcia/wyjœcia nawet dotycz¹cych terminala/ekranu tekstowego, nie mówi¹c ju¿ o operacjach graficznych. Tzw. standardowe wejœcie/wyjœcie pozostaje ci¹gle na poziomie "teletype" jako najmniejszego wspólnego mianownika. Jeœli potrzebne jest coœ wiêcej - np. proste wyœwietlenie paru punktów czy kresek na ekranie czy wyœwietlenie ramki z tekstem na œrodku(!) ekranu to de facto nawet uzyskanie aplikacji przenoœnej pomiêdzy ró¿nymi systemami "unix'owymi" nie jest zadaniem ca³kiem trywialnym. Co dopiero gdy program musi mieæ wersje pracuj¹ce pod DOS'em, Windows , X-windows czy unix'owym terminalu tekstowym!
Dla du¿ych (i bogatych) firm softwarowych czy innych zespo³ów programistów istniej¹ rozwi¹zania w postaci gotowych bibliotek GUI o zapewnionej przenoœnoœci. Nawet MFC Microsoftu posiada (podobno) wersje na platformê unix/Motif. S¹ to jednak rozwi¹zania kosztowne, zazwyczaj niedostêpne w postaci kodu Ÿród³owego co automatycznie ogranicza iloœæ potencjalnych platform. Przede wszystkim s¹ to jednak rozwi¹zania bardzo kosztowne i zwykle zbyt du¿ego kalibru by obci¹¿aæ nimi proste aplikacje. 
	Poniewa¿ odk¹d zacz¹³em programowaæ w C/C++ , robiê to na co najmniej dwóch platformach, a ostatnio na 5(!), postanowi³em rozwi¹zaæ ten problem na swoje potrzeby. Dopracowa³em siê zestawu modu³ów prostego interface'u graficznego razem obs³uguj¹cych, i DOS, i MS Windows, i unix/X-windows. Mo¿liwe jest tez stosunkowo ³atwe dopisanie kolejnych.
 
SK£AD PAKIETU
Maksymalna zawartoœæ dystrybucji pakietu obejmuje w obecnej wersji 0.9 nastêpuj¹ce pliki:
KATALOG ROOT:
*platform.h - plik nag³ówkowy s³u¿¹cy znormalizowanej detekcji u¿ytego kompilatora i platformy.
*symshell.h - plik nag³ówkowy interface'u . Wszystkie modu³y implementacyjne musz¹ byœ z nim zgodne.
*errorent.c - standardowa definicja zmiennej WB_error_enter_before_clean.
*inne pochodzace z powi¹zanych dystrybucji np graphs.
KATALOG X11:
*symshx11.c - plik implementuj¹cy funkcje SYMSHELL'a na platformie X-windows
*icon.h - plik nag³ówkowy dla ikony aplikacji w wersji X-windows
*wbx11ico.c - plik ze standardow¹ definicj¹ ikony dla aplikacji w wersji X-windows
KATALOG WIN:
*symshmsc.rc - wzorcowy plik zasobów dla aplikacji w wersji Win32 kompatybilny MS Visual C++ 4.0
*symshwin.c - plik implementuj¹cy funkcje SYMSHELL'a na platformie Win32 (testowany na Windows NT i Windows 95)
*symshwin.h ,win16ext.h, resources.h - lokalne definicje niezbêdne do kompilacji modu³u symshwin.c i jego zasobów.
*symshwin.ico - standardowa ikona aplikacji dla wersji Win32
*symshwin.rc - wzorcowy plik zasobów dla aplikacji w wersji Win32 kompatybilny z Borland 5.0
*winmain.c - plik przekszta³caj¹cy wywo³anie funkcji WinMain() aplikacji GUI Win32 na wywo³anie standardowej funkcji main(). Niekonieczny dla aplikacji CONSOLE Win32.
 
KATALOG GRX:
*symshgrx.c - plik implementuj¹cy funkcje SYMSHELL'a na platformy TURBOC i DJGPP z u¿yciem biblioteki graficznej GRX lub GRX2.0
KATALOG TXT:
*symshcur.c - plik implementuj¹cy funkcje SYMSHELL'a w pe³noekranowym trybie terminala tekstowego. Oczywiœcie rozdzielczoœci s¹ rzêdu 80x25 lub 132x40. W obecnej wersji nie zawiera juz w pelni funkcjonalnej implementacji.
*symshtxt.c - atrapa implementacji pos³uguj¹ca siê (prawie) standardowym wejœciem/wyjœciem i wyprowadzaj¹ca na ekran terminala jedynie informacje tekstow¹. Mo¿e te¿ byæ zupe³nie "g³ucha" co umo¿liwia ³atwe przerobienie (przez relinkowanie) aplikacji graficznej na zadanie obliczeniowe pracuj¹ce w tle.
KATALOG BIN:
*BC5.0/symshwin.obj,winmain.obj - skompilowany modu³ w wersji dla Win32 kompilatora Borland 5.0
*DJGPP/symshx11.o - skompilowany modu³ w wersji dla DJGPP (32 bit DOS)
*GRX/symshgrx.o - skompilowany modu³ w wersji dla DJGPP 2.0 . Biblioteka GRX 2.0 
*IRIX/symshx11.o - skompilowany modu³ w wersji dla systemu SG IRIX 6.x
*LINUX/symshx11.o - skompilowany modu³ dla systemu LINUX (i386)
*MSC4.0/symshwin.obj,winmain.obj - skompilowane modu³y w wersji dla Win32 kompilatora MS Visual C++ 4.0.
*MSVC4.0/symshmsc.res - skompilowany wzorcowy plik zasobow.
*TC/symshgx1.obj, TC/symshgx1.obj - skompilowany modu³ w wersji dla DOS BC/TC Bibliotekê GRX dla DOS trzeba mieæ z innego Ÿród³a.
zawarta jest w pe³nej dystrybucji DJGPP.
KATALOG TESTS:
*ro¿norodne programy testowe i demonstracyjne.
KATALOG DOCS:
*pliki dokumentacji w roŸnych formatach.
KATALOG GRAPHS:
*Pliki obiektowej biblioteki na¿êdzi programowania symulacji. Opisane w plikach
dokumentacyjnych GRAPHS.RTF lub analogicznych.
 
 
Jak dotychczas nie zaimplementowa³em SYMSHELL'a w grafice BGI Borlanda i sam ju¿ tego nie zrobiê, ale jeœli ktoœ bêdzie mia³ ochotê to chêtnie do³¹czê do dystrybucji, jak równie¿ inne, o których dotychczas nie pomyœla³em lub nie mia³em mo¿liwoœci np. ma MacOS. Chwilowo nie mam dostêpu do Linux'a wiêc nie proponuje modu³ów skompilowanych dla tego systemu jednak wersje CURSES i X11 powinny daæ siê tam skompilowaæ bez problemów.
UWAGA: 
Nie ka¿da wersja dystrybucyjna pakietu zawiera wszystkie pliki. W szczególnoœci wersje shareware nie zawieraj¹ kodów Ÿród³owych dla modu³ów Win32 i X11, a wersje demo nie zawieraj¹ nie wygasaj¹cych (bezterminowych) modu³ów skompilowanych dla tych platform.
 
OPIS INTERFACE'U SYMSHELL
	SYMSHELL jest zestawem funkcji implementowanych w jêzyku C, ale oczywiœcie mo¿liwych do u¿ycia równie¿ w C++ . Jego ogólna filozofia wywodzi siê z prostych programów symulacyjnych pisanych pod DOS'em, st¹d nazwa, ale nadaje siê te¿ do pisania prostych aplikacji przetwarzaj¹cych dane, wizualizuj¹cych je, a tak¿e innych, jeœli autorowi zale¿y na zachowaniu przenoœnoœci. Jedynym warunkiem jest zachowanie ogólnej struktury aplikacji wg. nastêpuj¹cego schematu:
	1)ustalenie parametrów pracy SYMSHELL'a
	2)otwarcie trybu/okna graficznego funkcj¹
		init_plot(...)
	3)g³ówna pêtla aplikacji u¿ywaj¹ca funkcji SYMSHELL'a
	 Dla poprawnej pracy. szczególnie na platformach okienkowych aplikacja MUSI 	
	 u¿ywaæ funkcji input_ready() , get_char() i flush_plot()
	 oraz reagowaæ na znaki specjalne:
		\b - zdarzenie myszy 
		\r - odrysowanie ekranu
		EOF - koniec wejœcia SYMSHELL'a 
	4)Zamkniêcie grafiki funkcja close_plot()
Funkcje s¹ opisane w kolejnoœci w jak w pliku nag³ówkowym "symshell.h" :
Funkcje konfiguruj¹ce i zarz¹dzaj¹ce:
int mouse_activity(int yes);
Ustala czy mysz ma byæ obs³ugiwana przez aplikacje. Zwraca poprzedni stan flagi. Jeœli yes=0 i jeœli aplikacja jest jedyn¹ "w³aœcicielk¹" ekranu to kursor nie pojawi siê lub zostanie wygaszony. W przeciwnym wypadku aplikacja jedynie przestaje odbieraæ zdarzenia myszy. Jeœli aplikacja przed otwarciem okna nie zadeklaruje czy chce u¿ywaæ myszy to mo¿e siê ona nie daæ zaaktywowaæ. Rozs¹dne jest wy³¹czenie aktywnoœci myszy na czas odbudowywania ekranu.
void set_background(unsigned char color);
Ustala indeks koloru do czyszczenia ekranu. Domyœlnym kolorem jest kolor 0 - czarny. Jeœli aplikacja nie zadeklaruje koloru t³a przed otwarciem okna lub zmienia go po otwarciu to efekty mog¹ byæ po³owiczne.
void buffering_setup(int yes);
Prze³¹czanie buforowania okna w pamiêci. Jeœli yes=0 to ka¿da operacja rysowania odbywa siê bezpoœrednio na ekranie (chyba ¿e to niemo¿liwe). Jeœli buforowanie jest w³¹czone to odœwie¿enie zawartoœci ekranu odbywa siê dopiero w momencie wywo³ania funkcji flush_plot(). W obecnej implementacji buforowanie mo¿na w³¹czyæ jedynie przed otwarciem okna graficznego. W œrodowiskach wielookienkowych zmniejsza to iloœæ przypadków kiedy konieczne jest odrysowanie ekranu przez aplikacje. W œrodowisku jednoekranowym tryb z buforowaniem mo¿e nie wspó³dzia³aæ w³aœciwie z obs³ug¹ myszy - szczególnie przy czêstym odœwie¿aniu ekranu. Na szczêœcie rzadko istnieje koniecznoœæ pisania takich aplikacji.
void shell_setup(char* Title, int Iargc, char* Iargv[]);
Przekazanie parametrów wywo³ania programu (funkcji main) do SYMSHELL'a . Pe³n¹ listê parametrów rozpoznawan¹ przez dan¹ implementacje mo¿na uzyskaæ podaj¹c parametr -h lub -help. Parametr Title s³u¿y do ustalenia tytu³u okna jeœli dana platforma dopuszcza tak¹ mo¿liwoœæ. Funkcja ta powinna byæ wywo³ywana przed funkcj¹ init_plot(). Jeœli nie zostanie wywo³ana to przyjmowane s¹ wartoœci domyœlne. 
Akceptowane przez SYMSHELL parametry wywo³ania maj¹ znormalizowan¹ postaæ :
	-nazwa+ lub -nazwa- .Pe³na lista zawiera:
	-input[+-] ->wy³¹czanie odbioru wejœcia. Tylko wersja tekstowa.
	-output[+-] ->wy³¹czanie standardowego wyjœcia. Tylko wersja tekstowa
	-mouse[+-] ->prze³¹czenie odbioru myszy.
	-async[+-] ->prze³¹czenie obs³ugi myszy w tryb asynchroniczny. Tylko GRX.
	-buffered[+-] ->prze³¹czanie buforowania wyjœcia graficznego.
	-traceenv[+-] ->w³¹czenie tekstowego œledzenia zdarzeñ (events). Tylko w X11.
	-mapped[+-] ->w³¹czenie trybu podwójnego zapisu w œrodowiskach okienkowych. Z punktu widzenia aplikacji jest on niezauwa¿alny, usprawnia jednak odœwie¿anie ekranu zwalniaj¹c w wiêkszoœci przypadków aplikacjê z tego obowi¹zku. Jednak ka¿da operacja ekranowa jest oko³o dwa razy wolniejsza bowiem odbywa siê jednoczeœnie na ekranie i na buforze w pamiêci.
void init_plot(int a, int b, int ca, int cb);
Uruchomienie trybu graficznego lub semigraficznego lub otwarcie graficznego okna aplikacji.
a - wymagana szerokoœæ ekranu w pikselach, b - wymagana wysokoœæ ekranu w pikselach, 
ca - iloœæ dodatkowych kolumn tekstu (w znakach) po prawej stronie okna,
cb - iloœæ dodatkowych wierszy tekstu (w znakach) u do³u okna.
Pe³ny wymagany rozmiar ekranu powstaje ze zsumowania tych wartoœci i jest zale¿ny od rozmiaru fontu stosowanego przez dan¹ implementacjê.
Faktyczny rozmiar okna mo¿e byæ wiêkszy od wymaganego, szczególnie w implementacjach pe³noekranowych.
Jeœli platforma dopuszcza zmianê rozmiaru okna przez u¿ytkownika aplikacji to fakt tej zmiany jest w miarê mo¿liwoœci maskowany przed aplikacj¹ przez zwielokrotnianie rozmiaru punktu, ale tylko do wartoœci ca³kowitych.
	Trzeba mieæ na uwadze ¿e czasem okno graficzne nakrywa siê z oknem tekstowym odbieraj¹cym standardowe wyjœcie, czasem okno graficzne uniemo¿liwia pracê standardowego wyjœcia, czasem (Windows) mo¿liwe jest takie zbudowanie aplikacji ze jest ona w ogóle pozbawiona funkcjonalnego standardowego wejœcia/wyjœcia.
void flush_plot();
Uzgodnienie zawartoœci ekranu z operacjami graficznymi SYMSHELL'a . Niektóre œrodowiska np X11 czy CURSES wymagaj¹ stosowania tej funkcji do prawid³owej pracy, w innych jest ona niezbêdna w trybie "buffered". W celu zachowania przenoœnoœci nie mo¿na zaniedbywaæ wywo³ywania tej funkcji, najlepiej pod koniec logicznego bloku wywo³añ graficznych. Zbyt czêste wywo³ania mog¹ jednak powodowaæ znaczny spadek wydajnoœci aplikacji poniewa¿ mog¹ byæ kosztowne.
void close_plot(); 
Zamkniêcie grafiki lub semigrafiki lub okna graficznego. Funkcjê tê nale¿y wywo³ywaæ dla przyzwoitoœci na koñcu programu, jednak prawid³owa implementacja SYMSHELL'a umieszcza j¹ w kolejce funkcji atexit w trakcie wywo³ania open_plot() co zazwyczaj jest wystarczaj¹ce. Wewn¹trz funkcji close_plot sprawdzana jest zmienna zadeklarowana jako:
	extern int WB_error_enter_before_clean/* =0 */;
Jeœli ma ona wartoœæ ró¿n¹ od 0 to funkcja interaktywnie prosi u¿ytkownika o potwierdzenie. Zmienna ta musi byæ gdzieœ zdefiniowana w kodzie aplikacji lub nale¿y dolinkowaæ modu³ errorent.c. Funkcjê tê mo¿na bezpiecznie wywo³ywaæ wielokrotnie, a nawet wywo³ywaæ j¹ mimo niepowodzenia funkcji open_plot(). Tylko pierwsze wywo³anie nastêpuj¹ce po zakoñczonym sukcesem otwarciu trybu graficznego wykonuje jakiekolwiek czynnoœci.
Funkcje odczytuj¹ce parametry wyœwietlania:
int screen_height(); 
int screen_width(); 
Ostateczne rozmiary okna po przeliczeniach parametrów funkcji init_plot() zawieraj¹ce tak¿e marginesy tekstowe. Pozwalaj¹ na pozycjonowanie w okolicach prawego dolnego rogu ekranu.
int char_height(char znak);
int char_width(char znak);
Aktualne rozmiary znaku w pikselach SYMSHELL'a, potrzebne do pozycjonowania tekstu. Rozmiar czcionki mo¿e pozornie maleæ z punktu widzenia aplikacji, jeœli u¿ytkownik powiêkszy okno graficzne. Nie nale¿y ich wiêc zapamiêtywaæ pomiêdzy wywo³aniami funkcji input_ready(), get_char(), flush_plot() .
Funkcje do czytania wejœcia:
int input_ready(); 
Nieblokuj¹ca funkcja sprawdzaj¹ca czy jest dostêpne wejœcie. Jeœli funkcja zwraca 1 to jest pewnoœæ ¿e nastêpuj¹ce potem wywo³anie funkcji get_char() nie mo¿e zastopowaæ aplikacji.
int get_char(); 
Funkcja odczytywania znaków sterowania. Znaki '\r' (carret return) , '\b' (bell) i EOF maj¹ specjalne znaczenie. Funkcja mo¿e zwracaæ wartoœci wiêksze od 255 jako kody znaków niestandardowych klawiatury (na razie niezdefiniowane) oraz informacje o wybraniu przez u¿ytkownika pozycji z menu (na razie tylko Windows).
Reakcj¹ aplikacji na znak EOF powinno byæ zakoñczenie trybu graficznego, reakcj¹ na znak '\r' powinna byæ odbudowa (odrysowanie) ekranu, a reakcj¹ na znak '\b' powinno byæ wywo³anie funkcji get_mouse_event(...). Jeœli aplikacja nie czyta odpowiednio czêsto wejœcia lub nie zareaguje prawid³owo na '\r' to efektu wizualne mog¹ byæ dziwne. Brak wywo³ania funkcji get_mouse_event(...) mo¿e powodowaæ utratê zdarzenia myszy lub powtórne zwrócenie przez funkcjê get_char() znaku '\b'.
int get_mouse_event(int* xpos, int* ypos, int* click);
Przepisuje wartoœci ostatniego zdarzenie myszy na zmienne których adresy pobiera z parametrów wywo³ania. 
	xpos - adres zmiennej na któr¹ przepisana zostanie wspó³rzêdna pozioma kursora.
	ypos - adres zmiennej na któr¹ przepisana zostanie wspó³rzêdna pionowa kursora.
	click -	adres zmiennej na któr¹ przepisana zostanie wartoœæ stanu przycisków myszy;
bit o wadze 1 oznacza przycisk lewy, bit o wadze 2 przycisk prawy, a bit o wadze 4 œrodkowy.
Funkcje wyprowadzaj¹ce tekst na ekran graficzny:
void print(int x, int y, char* format,...); 
void printc(int x, int y,
	unsigned char foreground, unsigned char background,
	char* format,...);
Funkcje wyprowadzaj¹ce tekst dzia³aj¹ce analogicznie do standardowej funkcji printf.
Parametry x i y oznaczaj¹ po³o¿enie lewego górnego rogu pierwszego znaku; "print" wyprowadza tekst w kolorach domyœlnych - zazwyczaj czarno na bia³ym, "printc" w kolorach o indeksach podanych parametrami "foreground" i "background". Aplikacja nie ma wp³ywu na rozmiar czcionki, mo¿e jednak policzyæ zawczasu rozmiar tekstu. (W Windows wyniki takich obliczeñ s¹ przybli¿one).
Funkcje rysuj¹ce:
Zestaw tzw prymitywów graficznych. Stosunkowo ³atwe by³oby jego rozbudowywanie. Zak³ada siê rysowanie i wype³nianie z poziomu aplikacji z u¿yciem pe³nych kolorów. Ditchering i szrafy mog¹ zostaæ zastosowane przez implementacje SYMSHELL'a w wypadku niedostêpnoœci na urz¹dzeniu wyjœciowym 256 kolorów.
Parametr "c" oznacza zawsze indeks koloru, parametry "x*","y*" oznaczaj¹ wspó³rzêdne ekranowe, parametr "r" promieñ okrêgu.
void plot(int x, int y, unsigned char c); 
Wyœwietlenie punktu na ekranie.
void line(int x1,int y1,int x2,int y2,unsigned char c);
Wyœwietlenie linii od punktu x1y1 do punktu x2y2 .
void circle(int x, int y, int r, unsigned char c);
Wyœwietlenie okrêgu o promieniu r w kolorze c, o œrodku w punkcie xy oczywiœcie.
void fill_circle(int x, int y, int r, unsigned char c);
Wyœwietla ko³o (pe³ne) o promieniu r w kolorze c .
void fill_rect(int x1,int y1, int x2,int y2, unsigned char c);
Wyœwietla prostok¹t wype³niony kolorem "c" o lewym górnym rogu w punkcie "x1 y1", a prawym dolnym w punkcie "x2 y2".
void fill_poly(int vx,int vy,
		const ssh_point points[], int 	number,
		unsigned char c);
Wype³nia kolorem "c" wielok¹t o "number" wierzcho³ków zdefiniowanych tablic¹ punktów "points" i przesuniêty o wektor "vx","vy" w stosunku do œrodka uk³adu wspó³rzêdnych ekranowych. W obecnej wersji dla biblioteki Curses wielok¹t nie jest wype³niany!
void clear_screen();
Czyœci ekran wype³niaj¹c go kolorem ustalonym funkcj¹ set_background() .
Operacje na kolorach:
void set_rgb(int color, int r, int g, int b);
Zmienia definicje sk³adowych koloru. Aktualna implementacja przyjmuje indeksy kolorów w zakresie 0..255 . Wiêksza iloœæ kolorów mog³aby byæ nieprzenoœna. Standardowo SYMSHELL inicjuje sk³adowe kolorów w ci¹g³¹ ska³ê barwn¹ przypominaj¹c¹ skalê temperaturow¹. Mo¿liwe s¹ jednak dowolne zmiany o ile umo¿liwia je platforma.
Parametr "color" oznacza indeks koloru, którego sk³adowe RGB maj¹ byæ zmienione. Pozosta³e parametry r,b,g s³u¿¹ do podania wartoœci w zakresach od 0 do 255. Nie nale¿y raczej zmieniaæ definicji koloru o indeksie 0 (BLACK:RGB=0.0.0) oraz o indeksie 255 (WHITE: RGB=255.255.255). W wypadku braku kolorów na urz¹dzeniu wyjœciowym implementacja SYMSHELL'a stara siê je zasymulowaæ, mo¿e jednak nie byæ w stanie zmieniæ definicji kolorów albo wykorzystaæ pe³nej informacji. Np na karcie VGA sk³adowe maj¹ zakresy 6-cio a nie 8-mio bitowe. Obecnie symulacji kolorów dokonuje wersja dla platformy CURSES, a wersja dla Win32 robi to de facto na bazie domyœlnego=systemowego ditheringu kolorów w Windows.
 
U¯YCIE SYMSHELL'A W APLIKACJACH
Poniewa¿ zestaw funkcji SYMSHELL'a s¹ jest stosunkowo nieliczny s¹ one zestawione w pojedyncze pliki Ÿród³owe. Ogranicza to zaœmiecanie przestrzeni nazw aplikacji pomocniczymi zmiennymi implementacji SYMSHELL'a i u³atwia ich u¿ycie, które polega na umieszczeniu plików nag³ówkowych pakietu w miejscu widocznym dla kompilatora C/C++ i odpowiedniego pliku symsh*.c lub symsh*.o* w zestawie plików projektu. Jeœli aplikacja nie definiuje w którymœ ze swoich w³asnych plików zmiennej WB_error_enter_before_clean nale¿y do³¹czyæ tak¿e plik errorent.c .
	W przypadku œrodowiska X-windows nale¿y tak¿e do³¹czyæ do projektu plik wbx11ico.c lub w³asny zgodny z plikiem icon.h . Zawiera on dane ikony wyœwietlanej w zastêpstwie zminimalizowanej aplikacji. Prawie dowolny typ grafiki binarnej mo¿na przekszta³ciæ na format tekstowy za pomoc¹ programy XV dostêpnego na zasadach licencji GNU na praktycznie ka¿d¹ platform¹ UNIX'ow¹.
	W przypadku stosowania wersji symshwin.c zadanie jest znacznie bardziej skomplikowane ze wzglêdu na specyficzny i nieprzenoœny charakter aplikacji dla okienkowych systemów Microsoftu. Poza do³¹czeniem pliku symshwin.c lub symshwin.obj do projektu aplikacji konieczne jest tak¿e zdefiniowanie odpowiednich zasobów aplikacji. Pakiet zawiera wzorcowy plik zasobów o nazwie symshwin.rc lub analogiczny przystosowany do kreatora zasobów z MS DEVELOPER STUDIO o nazwie symshmsc.rc. Do³¹czenie tego lub analogicznego (patrz dalej) pliku zasobów do projektu jest niezbêdne, ¿eby aplikacja u¿ywaj¹ca SYMSHELL'a dla Windows w ogóle otworzy³a okno graficzne! 
	Zalecany sposób modyfikacji zasobów to skopiowanie odpowiedniego wzorcowego pliku *.rc oraz pliku resources.h do katalogu zawieraj¹cego pliki Ÿród³owe aplikacji. Zale¿nie od kompilatora mo¿e te¿ okazaæ siê konieczne wskazanie œcie¿ki do pozosta³ych plików nag³ówkowych SYMSHELL'a w opcjach narzêdzia do modyfikacji zasobów. Nastêpnie nale¿y sprawdziæ czy aplikacja jest budowana i uruchamiana ze standardowym plikiem zasobów. Po przebrniêciu z sukcesem przez ten etap mo¿na przyst¹piæ do modyfikacji pliku zasobów na potrzeby konkretnej aplikacji. Mo¿na zmieniaæ nastêpuj¹ce zasoby:
Akcelerator - dowolnie, pod warunkiem ¿e nazwa zasobu bêdzie zgodna z nazw¹ aplikacji podan¹ w ³añcuchu IDS_APPNAME.
Ikona - dowolnie, pod warunkiem, ¿e bêdzie zdefiniowana ikona 16x16 i 32x32, a tak¿e, ¿e nie zostanie zmieniony identyfikator ikony aplikacji - IDI_APPICON.
Menu - dowolnie, pod warunkiem ¿e nazwa zasobu bêdzie zgodna z nazw¹ aplikacji podan¹ w ³añcuchu IDS_APPNAME. Identyfikatory wybranych pozycji s¹ przekazywane do aplikacji za poœrednictwem funkcji get_char(), nale¿y wiêc uwa¿aæ by by³y wiêksze od 255. Kreator zasobów z MS Visual C++ generuje te wartoœci poczynaj¹c od liczby 40004.
Tablica ³añcuchów - mo¿e byæ dowolnie rozbudowywana, ale przenoœna aplikacja SYMSHELL'a mo¿e i musi skorzystaæ jedynie z dwóch wartoœci: ISD_APPNAME oraz IDS_DESCRIPTION.
Informacja o wersji - nale¿y wype³niæ danymi aplikacji. Sam SYMSHELL na razie nie u¿ywa tych danych, niewykluczone jednak ¿e robi¹ to narzêdzia systemowe.
 
OBECNY STATUS I PERSPEKTYWY ROZWOJU
	SYMSHELL zosta³ zaprojektowany pocz¹tkowo na potrzeby aplikacji pisanych przez autora w Instytucie Studiów Spo³ecznych i Instytucie Botaniki Uniwersytetu Warszawskiego.
Obecnie za jego pomoc¹ utworzono ju¿ kilka aplikacji, które ci¹gle s¹ rozwijane, a wraz z nimi bêd¹ te¿ rozwijane implementacje SYMSHELL'a. Pakiet zostanie te¿ wykorzystany jako narzêdzie pomocnicze w Warsztatach z Programowania Modeli, które po raz pierwszy odbêd¹ siê w Instytucie Studiów Spo³ecznych w semestrze letnim 1997. Bêdzie te¿ udostêpniony magistrantom Zak³adu Systematyki Roœlin realizuj¹cym prace o profilu "komputerowy".
Poza tym planowane jest udostêpnienie publiczne poprzez Internet wersji demo i taniej wersji shareware dla celów edukacyjnych i naukowych. Niezale¿nie od tego ka¿da polskojêzyczna (na razie - ze wzglêdu na dokumentacjê) osoba fizyczna mo¿e pertraktowaæ z autorem na temat zakupu pe³nej imiennej wersji zawieraj¹cej kod Ÿród³owy.
Kontakt z autorem mo¿na uzyskaæ pod nastêpuj¹cymi adresami:
	borkowsk@bot.strouw.edu.pl 
	borkowsk@samba.iss.uw.edu.pl
