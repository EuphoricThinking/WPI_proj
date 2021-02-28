#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

/*
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ BOULDER   DASH ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ *
			                 	Zemsta Rockforda
                                                        ____
                                                       /.  .\
                                                      |  \/ |
                                                       \___/
                                                       __|__
                                                     /|     |\
                                                    / |     | \
                                                       \____/
                                                     $$$$\$$\$
						 $$$$$$$$$|$$|$$$
					     $$$$$$$$$$$$$$$$$$$$$$$
					   $$$$$$$$$$$$$$$$$$$$$$$$$$$

					(Rockford siedzący na diamentach)


	Program autorstwa Agaty Momot do przeprowadzenia rozgrywki w Boulder Dash. Jako wejście przyjmuje
plik z informacją o liczbie kolumn i wierszy, przygotowaną planszę oraz wykaz ruchów użytkownika.
Dodatkowo zaimplementowano tekstowy interfejs użytkownika (TUI). Standardowa kompilacja:
	gcc @opcje -lncurses ./program -o program
natomiast w przypadku wykorzystywania TUI:
	gcc @opcje -DTUI -lncurses ./program -o program
TUI testowano za pomocą wzoru polecenia:
	cat plansza.in - | ./program

*/


//Struktura definiująca pozycję w wierszu (x) oraz kolumnie (y) w danej tablicy dla danej liczby
typedef struct pozycja{
	int x;
	int y;
} pozycja;


/*Struktura przechowująca:
	- pozycja r:	    pozycja Rockforda oznaczonego jako '@';
	- bool jest_r:	    informacja o obecności Rockforda na planszy;
	- int diamenty:	    liczba diamentów na planszy;
	- int **pl;         wskaźnik do wskaźników, służący do przechowania dwuwymiarowej
			    tablicy opisującej rozmieszczenie poszczególnych obiektów
			    na wczytanej planszy;
*/
typedef struct plansza{
	pozycja r;
	bool jest_r;
	int diamenty;
	int **pl;
} plansza;



//Procedura wypisująca zawartość tablicy (przekazanej jako wskaźnik do wskaźników)
//na podstawie liczby wiersz (w) i kolumn (k)
void printer(int w, int k, int **tab) {
	printf("%d %d\n", w, k);
	for (int i =0; i< w; i++) {
		for (int j = 0; j< k; j++) {
			putchar(tab[i][j]);
		}
		printf("\n");
	}
}


/*Procedura sprowadzająca planszę do stanu stabilnego na podstawie
liczby wierszy (w), kolumn (k), wskaźnika do wskaźników odnoszącego się
do zawartości planszy. W przypadku stosowanie TUI, wykorzystywane są
informacje o pozycji pierwszej kolumny i pierwszego wiersza. Liczby kk i kp
oznaczają numer pierwszej i ostatniej kolumny, w której należy przeprowadzić
sprowadzanie planszy do stanu stabilnego, co umożliwia sprawdzanie pojedynczych
kolumn po opuszczeniu pola przez Rockforda bądź po przesunięciu kamienia.
Przed rozpoczęciem gry stabilizowana jest cała plansza.
*/
void stabilny(int wiersze, int kk, int kp, int **tab, int pierwszy_w, int pierwsza_k) {
	if (pierwszy_w + pierwsza_k == 0) {}    //Uciszenie ostrzeżenia w kompilacji o niewykorzystanych
						//zmiennych w przypadku braku TUI
	for (int i = wiersze-2; i > 0; i--) {        //Stabilizacja wierszy i kolumn ze skałami jest zbędna,
		for (int j = kk-2; j >= kp; j--) {   //któ©e można wykluczyć m.in. za pomocą i>0 (dla wierszy)
			if (tab[i][j] == 'O' || tab[i][j]== '$') {
				int w = i;
				while (tab[w+1][j] == ' '){
					tab[w+1][j] = tab[w][j];
					tab[w++][j] = ' ';

					#ifdef TUI
						int i1 = tab[w-1][j];
						int i2 = tab[w][j];
						int nowe_w = w-1 + pierwszy_w;
						int nowa_k = j + pierwsza_k;
						mvprintw(nowe_w, nowa_k, "%c", (char)i1);
						mvprintw(nowe_w+1, nowa_k, "%c", (char)i2);
					#endif

				}
			}
		}
	}
}

/*Procedura uzupełniająca strukturę przechowującą dane wczytanymi informacjami:
o obecności Rockforda, jego pozycji, liczbie diamentów. Wypełnia planszę wczytanymi znakami,
wczytując plik. Wykorzystuje wskaźnik do struktury plansza oraz liczbę wierszy (w).
*/
void wczytaj(int w, plansza *d) {
	int c;
	int wiersz = 0;
	int kolumna = 0;
	int diamenty = 0;
	d->jest_r = false;
	while (wiersz != w) {
		while ((c=getchar()) != '\n') {
			if (c == '@') {
				d->r.x = wiersz;
				d->r.y = kolumna;
				d->jest_r = true;
			}
			d->pl[wiersz][kolumna++] = c;
			if (c == '$') diamenty++;
		}
		wiersz++;
		kolumna = 0;
	}
	d->diamenty = diamenty;
}


/* Procedura przesuwająca Rockforda na wskazane miejsce, zbierająca diamenty, przekopująca ziemię,
przesuwająca kamienie i decydująca o opuszczeniu przez Rockforda całej planszy. Po aktualizacji
pozycji i zdefiniowaniu TUI, wyświetla wskazane pola. Stabilizuje planszę we wskazanych kolumnach.
Jako parametry przyjmuje wskaźnik do struktury plansza, cyfrę definiującą przesunięcie w pionie (pion),
cyfrę definiującą przesunięcie w poziomie (poziom), liczbę wierszy w planszy (w) oraz opcjonalnie
wykorzystywane numery pierwszego wiersza i pierwszej kolumny dla TUI.
*/
void ruch(plansza *d, int poziom, int pion, int pierwszy_w, int pierwsza_k, int w) {
	int nowe_dx = d->r.x + poziom;
	int nowe_dy = d->r.y + pion;
	int dx = d->r.x;
	int dy = d->r.y;
	if (pierwszy_w + pierwsza_k == 0){}       //Uciszenie ostrzeżeń o niewykorzystanych zmiennych

	#ifdef TUI
	int pokaz_dx = dx + pierwszy_w;
	int pokaz_dy = dy + pierwsza_k;
	int pokaz_nowe_dx = nowe_dx + pierwszy_w;
	int pokaz_nowe_dy = nowe_dy + pierwsza_k;
	#endif

	if (d->pl[nowe_dx][nowe_dy] == ' ' || d->pl[nowe_dx][nowe_dy] == '+'){
		d->pl[nowe_dx][nowe_dy] = '@';
		d->pl[dx][dy] = ' ';
		d->r.y += pion;
		d->r.x += poziom;

	} else if (d->pl[nowe_dx][nowe_dy] == '$') {
		d->pl[nowe_dx][nowe_dy] = '@';
		d->pl[dx][dy] = ' ';
		d->diamenty--;
		d->r.y += pion;
		d->r.x += poziom;

	} else if (d->pl[nowe_dx][nowe_dy] == 'O' && d->pl[nowe_dx][d->r.y + 2*pion] == ' ') {
		d->pl[nowe_dx][nowe_dy] = '@';
		d->pl[nowe_dx][d->r.y + 2*pion] = 'O';
		d->pl[dx][dy] = ' ';
		stabilny(w, d->r.y + 2*pion + 2, d->r.y + 2*pion, d->pl, pierwszy_w, pierwsza_k);

		#ifdef TUI
		int i4 = d->pl[dx][dy];
		int i5 = d->pl[nowe_dx][nowe_dy];
		int i6 = d->pl[nowe_dx][d->r.y + 2*pion];
		mvprintw(pokaz_dx, pokaz_dy, "%c", (char)i4);
		mvprintw(pokaz_nowe_dx, pokaz_nowe_dy, "%c", (char)i5);
		mvprintw(pokaz_nowe_dx, d->r.y + 2*pion + pierwsza_k, "%c", (char)i6);
		#endif

		d->r.y += pion;
		d->r.x += poziom;

	} else if (d->pl[nowe_dx][nowe_dy] == 'X' && d->diamenty == 0) {
		d->jest_r = false;
		d->pl[dx][dy] = ' ';
	}

	stabilny(w, dy + 2, dy, d->pl, pierwszy_w, pierwsza_k);

	#ifdef TUI
		int i1 = d->pl[dx][dy];
		int i2 = d->pl[nowe_dx][nowe_dy];
		mvprintw(pokaz_dx, pokaz_dy, "%c", (char)i1);
		mvprintw(pokaz_nowe_dx, pokaz_nowe_dy, "%c", (char)i2);
	#endif
}


#ifdef TUI
/*Procedura wyświetlająca na początku całą planszę dla zdefiniowanego TUI, wyłącznie dla którego
jest wykorzytywana. Przyjmuje wskaźnik do wskaźników (tablicę 2D), liczbę wierszy (w)
i liczbę kolumn (k) dla planszy oraz numer pierwszej i ostatniej kolumny dla TUI.
*/
void wyswietl_plansze_pierwszy_raz(int w, int k, int pierwszy_w, int pierwsza_k, int **tab) {
        for (int i = 0; i < w; i++) {
                int nowe_w = pierwszy_w + i;
                for (int j = 0; j < k; j++) {
                        int nowe_k = pierwsza_k + j;
                        int i1 = tab[i][j];
                        mvprintw(nowe_w, nowe_k, "%c", (char)i1);
                }
        }
        refresh();
}
#endif


/*Procedura sprowadzająca początkową planszę do stanu stabilnego i wyświetlająca rezultat.
Przyjmuje i interpretuje polecenia od użytkownika; po każdym ruchu następuje stabilizacja
przeprowadzona w funkcji ruch
*/
void gra(int w, int k, plansza *d, int pierwszy_w, int pierwsza_k) {
	int g;
	stabilny(w, k, 1, d->pl, pierwszy_w, pierwsza_k);   //Nie sprawdza kolumny zawierającej skały

	#ifdef TUI
		wyswietl_plansze_pierwszy_raz(w, k, pierwszy_w, pierwsza_k, d->pl);
		g = getch();
	#else
		printer(w, k, d->pl);
		g = getchar();
	#endif

	while (g != EOF && g != 27) {
		if (d->jest_r) {
			if (g == 'a' || g == KEY_LEFT) {
				ruch(d, 0, -1, pierwszy_w, pierwsza_k, w);
			} else if (g == 'w' || g == KEY_UP) {
				ruch(d, -1, 0, pierwszy_w, pierwsza_k, w);
			} else if (g == 's' || g == KEY_DOWN) {
				ruch(d, 1, 0, pierwszy_w, pierwsza_k, w);
			} else if (g == 'd' || g == KEY_RIGHT) {
				ruch(d, 0, 1, pierwszy_w, pierwsza_k, w);
			}
		}
		if (g == '\n') {

			#ifdef TUI
				refresh(); //Nie jest niezbędne przy cbreak(), jednak zachowuje analogię
					   //dla standardowej treści zadania
			#else
				printer(w, k, d->pl);
			#endif

		}

	#ifdef TUI
		g = getch();
	#else
		g = getchar();
	#endif

	}
}


//Funkcja tworząca dwuwymiarową tablicę - wskaźnik do wskaźników - na podstawie liczby kolumn i wierszy
int** stworz_2d(size_t wiersz, size_t kolumna, int w) {
	int **pl = malloc(wiersz*sizeof( *pl));
	for (int i = 0; i<w; i++) {
		pl[i] = malloc(kolumna*sizeof(pl[i]));
	}
	return pl;
}


/*Procedura zwalniająca dynamicznie zaalokowaną pamięć po zakończeniu wykonywania poleceń od użytkownika.
Jako argumenty przyjmuje wskaźnik do wskaźników (tablicę 2D) oraz liczbę wierszy (w).
*/
void mesjasz(int **z_Egiptu, int w){
	for (int Izraelitow = 0; Izraelitow < w; Izraelitow++) {
		free(z_Egiptu[Izraelitow]);
	}
	free(z_Egiptu);
}


#ifdef TUI
/*Procedura inicjalizująca ekran dla TUI
*/
void inicjalizacja() {
	initscr();
	noecho();
	keypad(stdscr, true);
	cbreak();
	curs_set(0);
}


/*Procedura zamykająca ekran dla TUI
*/
void zamknij_interfejs() {
	endwin();
}
#endif




int main(void) {
	size_t wiersz, kol;
	scanf("%zu %zu\n", &wiersz, &kol);
	int w = (int)wiersz;
	int k = (int)kol;
	plansza boulder_dash;
	boulder_dash.pl = stworz_2d(wiersz, kol, w);
	wczytaj(w, &boulder_dash);

	#ifdef TUI
	inicjalizacja();
	int pierwszy_wiersz = (LINES- w)/2;
	int pierwsza_kolumna = (COLS - k)/2;
	gra(w, k, &boulder_dash, pierwszy_wiersz, pierwsza_kolumna);
	zamknij_interfejs();
	#else
	gra(w, k, &boulder_dash, 0, 0);
	#endif

	mesjasz(boulder_dash.pl, w);   //Autorowi zależało na mejsaszu rock.fordzie :c
	return 0;
}
