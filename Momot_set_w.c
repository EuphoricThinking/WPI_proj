#include <stdio.h>
#include <stdbool.h>
#define STOL 12
#define TALIA 81
#define CECHY 4
#define SET 3

/*Program autorstwa Agaty Momot, przeprowadzający rozgrywkę seta. Karty do gry wczytuje z pliku,
w którym poszczególne cechy kart przedstawiono za pomocą cyfr 1, 2 i 3. Wyświetla kolejne dobieranie kart,
znalezione sety oraz zawartość stołu według zasad gry w zabawę nieprzeciętnie absorbującą, niewypowiedzianie
uzależniającą i niezmiernie nienawidzoną przez studentów, którzy nie zaznali uroku wspomnianej rozrywki
przed rozpoczęciem pracy nad zadaniem zaliczeniowym.

Cześć i chwała setowi! ~~~ Agata Momot
*/





/*Struktura do zapisu kart o polach:
	- tablica liczb całkowitych *tab* do zapisu cech;
	- boolean *set* określające, czy dana karta tworzy set z wybranymi kartami na stole.
*/
typedef struct karta {
	int tab[CECHY];
	bool set;
} karta;



/*Struktura reprezentująca stół o polach:
	- tablica kart (zdefiniowanych struktur) *tab* o długości uwzględniającej możliwość
występienia 20 kart bez seta, przechowująca karty na stole;
	- liczba całkowita *ile_kart*, zliczająca liczbę kart na stole.
*/
typedef struct stol {
	karta tab[STOL+9];
	int ile_kart;
} stol;



/*Struktura talia o polach:
	- tablica kart (zdefiniowanych struktur) *tab*, przechowująca karty wczytane z danych wejściowych;
	- liczba całkowita *ile_kart*, zliczająca niewykorzystane kart;
	- liczba całkowita *wykorzystane*, zliczająca wykorzystane karty.
*/
typedef struct talia {
	karta tab[TALIA];
	int ile_kart;
	int wykorzystane;
} talia;




//Wczytuje karty z pliku; tworzy talię kart, zwraca liczbę wczytanych kart
int czytaj(karta tab[]) {
	int ile_kart = 0;         //Zlicza wczytane karty
	int c;			  //Oznaczenie dla znaku wczytanego przez getchar()
	int w_tab = 0;            //Zlicza karty w talii
	int w_karcie = 0;	  //Zlicza wczytane cechy
	karta k;		  //Karta do nadpisywania cech i zapisywania
	k.set = false;		  //Oznaczenie początkowych kart jako niewykorzystanych w secie

	while ((c = getchar()) != EOF) {

		//Rozpoznaje cechy wczytanych kart
		if ((c == '3') || (c == '2') || (c == '1')) {
			k.tab[w_karcie++] = c-'0';	//Zapisuje cechy jako liczby całkowite
		}

		//Dodaje kartę do talii
		if (w_karcie == CECHY) {
			tab[w_tab++] = k;
			w_karcie = 0;
			ile_kart++;
		}
	}
	return ile_kart;
}


//Tworzy pierwszy stół do rozgrywki na podstawie wcześniej wczytanych kart; aktualizuje zawartość
//kart w talii oraz na stole.
void pierwszy_stol(stol* stol_kart, talia* talia_kart) {

	//Zapis w przypadku danych wejściowych przedstawiających ponad 12 kart
	if (talia_kart->ile_kart > 12) {
		for (int i = 0; i<STOL; i++) {
			stol_kart->tab[i] = talia_kart->tab[i];
		}
		talia_kart->ile_kart -= STOL;
		talia_kart->wykorzystane = STOL;
		stol_kart->ile_kart = STOL;

	//Zapis w przypadku danych wejściowych przedstawiających mniej niż 12 kart bądź dokładnie 12 kart
	} else {
		for (int i = 0; i < talia_kart->ile_kart; i++) {
			stol_kart->tab[i] = talia_kart->tab[i];
		}
		stol_kart->ile_kart = talia_kart->ile_kart;
		talia_kart->ile_kart -= talia_kart->ile_kart;
	}
}


//Sprawdza, czy dane cechy spełniają warunki do utworzenia seta; zwraca true w przypadku zgodności
bool czy_set_pozycja(int a, int b, int c) {
	if ((a==b && b==c) || (a!=b && b!=c && a!=c)) {
		return true;
	} else {
		return false;
	}
}


//Sprawdza, czy dane karty tworzą set; w przypadku znalezienia seta - zwraca true
bool czy_set_karty(karta a, karta b, karta c) {
	int ile_true = 0;
	for (int i = 0; i<CECHY; i++) {
		if (czy_set_pozycja(a.tab[i], b.tab[i], c.tab[i])) {
			ile_true++;
		}
	}
	return (ile_true==CECHY);
}


//Znajduje set na stole; w przypadku znalezienia seta oznacza wykorzystane karty,
//zapisuje set w tablicy kart wynik[] oraz zwraca true
bool znajdz_set(karta wynik[], karta stol_kart[], int ile_kart) {
	for (int i = 0; i<ile_kart-2; i++) {
		for (int j = i+1; j<ile_kart-1; j++) {
			for (int k = j+1; k<ile_kart; k++) {
				if (czy_set_karty(stol_kart[i], stol_kart[j], stol_kart[k])) {
					wynik[0] = stol_kart[i];
					stol_kart[i].set = true;
					wynik[1] = stol_kart[j];
					stol_kart[j].set = true;
					wynik[2] = stol_kart[k];
					stol_kart[k].set = true;
					return true;
				}
			}
		}
	}
	return false;
}


//Przesuwa karty tworzące set na koniec stołu
void przesun(karta tab[], int lim) {
	int i = 0;
	int j = 0;
	while (j<lim) {
		if (tab[j].set == true) {
			j++;
		} else {
			karta pom = tab[i];
			tab[i++] = tab[j];
			tab[j++] = pom;
		}
	}
}


//Wyświetla kartę. Ze względu na nazwę stosowanej funckji w c, zrezygnowano
//z polskiej nazwy definiowanej funkcji
void print_card(karta k) {
	for (int i = 0; i<CECHY; i++) {
		printf("%d", k.tab[i]);
	}
}


//Wyświetla zawartość talii bądź stołu
void print_line(karta tab[], int lim) {
	for (int i = 0; i<lim; i++) {
		printf(" ");
		print_card(tab[i]);
	}
	printf("\n");
}


//Procedura przeprowadzająca rozgrywkę w set. Wyświetla rezultat gry.
void rozgrywka(stol* stol_kart, talia* talia_kart) {
	karta wynik[SET];	//Przechowuje set
	bool gra = true;
	while (gra) {
		printf("=");
		print_line(stol_kart->tab, stol_kart->ile_kart);

		//Modyfikuje stół w przypadku znalezienia seta; wyświetla set
		if (znajdz_set(wynik, stol_kart->tab, stol_kart->ile_kart)) {
			printf("-");
			print_line(wynik, SET);
			przesun(stol_kart->tab, stol_kart->ile_kart);
			stol_kart->ile_kart -= SET;

			//Dobiera karty w przypadku liczby kart na stole mniejszej niż 12
			if (talia_kart->ile_kart != 0 && stol_kart->ile_kart < STOL) {
/*				Prawdopodobnie istnieje możliwość zastąpienia procedurą poniższego fragmentu
				wraz z analogiczną częścią kodu w else, jednak posługiwanie się zagnieżdżonymi
				wskaźnikami przekracza zakres umiejętności autora w okresie tworzenia programu.

				Poniższy kod aktualizuje zawartość stołu, dodając jako trzy ostatnie karty
				trzy pierwsze niewykorzystane karty z talii. Zmienia liczbę kart w talii, na stole
				oraz liczbę kart wykorzystanych.
*/
				stol_kart->tab[stol_kart->ile_kart] = talia_kart->tab[talia_kart->wykorzystane];
				stol_kart->tab[stol_kart->ile_kart + 1 ]= talia_kart->tab[talia_kart->wykorzystane + 1];
				stol_kart->tab[stol_kart->ile_kart + 2] = talia_kart->tab[talia_kart->wykorzystane + 2];
				stol_kart->ile_kart += SET;
				talia_kart->ile_kart -= SET;
				talia_kart->wykorzystane += SET;
			}

		//Dobiera karty bądź kończy grę w przypadku braku seta
		} else {
			if (talia_kart->ile_kart != 0) {
				printf("+\n");
				stol_kart->tab[stol_kart->ile_kart] = talia_kart->tab[talia_kart->wykorzystane];
				stol_kart->tab[stol_kart->ile_kart + 1] = talia_kart->tab[talia_kart->wykorzystane + 1];
				stol_kart->tab[stol_kart->ile_kart + 2] = talia_kart->tab[talia_kart->wykorzystane + 2];
				stol_kart->ile_kart += SET;
				talia_kart->ile_kart -= SET;
				talia_kart->wykorzystane += SET;
			} else {
				gra = false;
			}
		}
	}
}



int main(void) {
	talia talia_kart;
	talia_kart.ile_kart = czytaj(talia_kart.tab);
	stol stol_kart;
	pierwszy_stol(&stol_kart, &talia_kart);
	rozgrywka(&stol_kart, &talia_kart);
	return 0;
}
