#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>


#define LITERY 25  //indeks ostatniej litery w tablicy
#define INC 'a'
#define DJZ 'b'
#define ADD 'c'
#define JMP 'd'
#define HLT 'e'
#define CLR 'f'

#define ODJ 26  //Odejmowanie -1 w przypadku DEC
#define DOD 27  //Dodawanie +1 w przypadku INC

#define ILE_CYFR 4   //Liczba cyfr w komórce według systemu o podstawie 10000


/*
W obliczu pandemii SARS-CoV-2, znanego powszechniej pod angielską nazwą
,,severe acute respiratory syndrome coronavirus 2", naród polski musiał zmierzyć się
z nieznanymi dotychczas ograniczeniami dotyczącymi zarówno każdego obywatela,
jak i wszystkich przedsiębiorstw czy instytucji. Szczególnie obciążone
przeciwnościami związanymi ze zminimalizowaniem bezpośrednich interakcji
międzyludzkich zdają się być placówki oświatowe, co oznacza chwilę próby
dla polskiego systemu edukacji.

W trosce o rozwój obywateli, zdecydowaliśmy się wesprzeć uniwersytety i szkoły
dzielnie przeciwstawiające się niedogodnościom wynikającym ze zdalnego nauczania.
Przedstawiamy prototyp alfabetycznego kalkulatora: Alfabeusza 2077, aby niósł
kaganek oświaty wśród młodzieży polskiej i niczym Prometeuesz podarował
człowiekowi to, co niezbędne do życia - matematykę.



Pierwszy autor:   Agata
Drugi autor:      Sława
Trzeci autor:     Momot
*/







/*Struktura przedstawiająca liczbę w sysytemie o podstawie 10000, zawierająca:
- int* komorka - wskaźnik do tablicy czterocyfrowych komórek zawierających liczbę;
- int ile_kom - długość tablicy;
- size_t rozmiar - rozmiar tablicy;
*/
typedef struct liczba{
	int* komorka;
	int ile_kom;
	size_t rozmiar;
} liczba;


/* Struktura zawierająca informacje uwzględnionwe w instrukcjach
- char polecenie - zdefiniowana stała określająca polecenie;
- int indeks_zmiennej - indeks zmiennej w tabeli struktur liczba;
- int adres_pocz - adres komórki rozpoczynającej instrukcję Powtarzaj w tablicy
kodu maszynowego;
- int adres_kon - adres komórki pierwszej komórki po instrukcji Powtarzaj w tablicy
kodu maszynowego;
- int indeks_zmiennej_dod - indeks zmiennej w tabeli struktur liczba,
którą dodajemy do danej liczby
*/
typedef struct instrukcja {
	char polecenie;
	int indeks_zmiennej;

	int adres_pocz;
	int adres_kon;

	int indeks_zmiennej_dod;
} instrukcja;


/*
Struktura przechowująca kod maszynkowy
- instrukcja* instr - wskaźnik do tablicy instrukcji;
- int rozm - aktualny rozmiar;
*/
typedef struct kod {
	instrukcja* instr;
	int rozm;
} kod;




//Funkcja inicjalizująca tablicę liczb na podstawie wskaźnika NULL
void stworz_zmienne(liczba** tab) {
	*tab = realloc(*tab, (DOD + 1)*(sizeof(**tab)));
	for (int i = 0; i < DOD + 1; i++) {
		liczba kom;
		kom.komorka = calloc(ILE_CYFR, sizeof(*kom.komorka));
		kom.ile_kom = 1;
		kom.rozmiar = ILE_CYFR*sizeof(*kom.komorka);
		(*tab)[i] = kom;
	}
	(*tab)[ODJ].komorka[0] = -1;
	(*tab)[DOD].komorka[0] = 1;
}

//Zwraca liczbę cyfr krótszej liczby
int min(liczba a, liczba b) {
	if (a.ile_kom > b.ile_kom) return b.ile_kom;
	else return a.ile_kom;
}

//Dzielenie poprzez przemnażanie, by uniknąć ,,kosztownej operacji"
int dzielenie(int a) {
	int i = 1;
	while (10000*i <= a) {
		i++;
	}
	return --i;
}

/*Funkcja pomocnicza, dodająca do siebie dwie komórki z uzwględnieniem
i aktualizacją pomocniczej zmiennej zawierającą resztę przekazywaną następnym
komórkom*/
int dodaj(int w, int m, int* p) {
	int liczbunia;
	int wynik = w + m + *p;
	if (wynik >= 10000) {
		*p = dzielenie(wynik);
		liczbunia = wynik - 10000*(*p);
	} else {
		liczbunia = wynik;
		*p = 0;
	}
	return liczbunia;
}

//Procedura zwiększająca rozmiar tablicy cyfr w przypadku dodawania nowej komórki,
void zwieksz(liczba* a, int i) {
	if ((size_t)i*ILE_CYFR*sizeof(int) >= a->rozmiar) { //Zwiększa rozmiar w przypadku potrzeby
		size_t rozmiarek = (a->rozmiar)*2 + 1;
		a->komorka = realloc(a->komorka, rozmiarek);
		a->rozmiar = rozmiarek;
	}
}


//Procedura umożliwiająca dodawanie reszty z uzyskanej z poprzednich komórek
void dokoncz_ogon(liczba* a, int* pom, int* i) {
	if (*i == a->ile_kom) {   //Jeśli dodajemy nową komórkę poza zakresem
		zwieksz(a, *i);
		a->komorka[(*i)++] = *pom;
		a->ile_kom++;
		*pom = 0;
	} else {
		a->komorka[*i] = dodaj(a->komorka[*i], 0, pom);
		(*i)++;
	}
}


//Procedura dodająca do liczby a liczbę b bez zmiany wartości b
void add(liczba* a, liczba* b) {
	int pom = 0;
	int i = 0;
	int mniejsze = min(*a, *b);
	while (i < mniejsze) {   //Dodawanie komórek w zakresie ,,krótszej" liczby
		a->komorka[i] = dodaj(a->komorka[i], b->komorka[i], &pom);
		i++;
	}
	if (mniejsze == a->ile_kom) {  //Jeśli liczba a jest mniejsza od b
		while (i < b->ile_kom) {    //Dodawanie komórek liczby b
			zwieksz(a, i);
			a->komorka[i] = dodaj(0, b->komorka[i], &pom);
			a->ile_kom++;
			i++;
		}
	}
	while (pom != 0) {  //Dodawanie reszty
			dokoncz_ogon(a, &pom, &i);
	}
}

//Procedura przeprowadzająca operację --Liczba
void odejmij(liczba* a) {
	if (a->ile_kom == 1) {   //Jeśli jednocyfrowa
		a->komorka[0]--;
	} else {
		if (a->komorka[0] != 0) {   //Jeśli ostania cyfra nie jest równa zero
			a->komorka[0]--;
		} else {
			bool odj = true;
			int i = 0;
			while (odj) {       //Odejmuje jeden aż do pozycji pierwszej niezerowej komórki
				if (a->komorka[i] == 0) {
					a->komorka[i++] = 9999;
				} else {
					a->komorka[i++]--;
					odj = false;
				}
			}
			while (a->komorka[a->ile_kom - 1] == 0) {  //Aktualizuje liczbę komórek
				a->ile_kom--;
			}
		}
	}
}

//Funckja zwracająca indeks odpowiedniej zmiennej w tablicy
int przelicz_indeks(int c) {
	return c - 'a';
}

//Funkcja pobierająca nowy znak i zwracająca indeks znaku w tablicy
int zmienna() {
	int znak = getchar();
	return przelicz_indeks(znak);
}

//Funckja zwracająca nowy rozmiar w tablicy, zabezpieczająca przed przekroczeniem
//INT_MAX na podstawie obecnego rozmiaru i rozmiaru struktury
int nowy_rozm(int rozm, int poj) {
	if (rozm + poj < INT_MAX/2) {  //Wartości int przekraczające bezpośrednio INT_MAX
		return 2*poj + 2*rozm;       //przyjmują ujemny znak
	} else {
		return INT_MAX - 1;
	}
}

//Funkcja wyliczająca aktualny rozmiar struktury na podstawie aktualnego indeksu
//i rozmiaru elementu danej struktury
int obecny_rozm(int poj, int ind) {
	return ind*poj;
}


//Funkcja inicjalizująca instrukcję i uzupełniająca jej własności na podstawie
//argumentów
instrukcja stworz_instr(char pol, int ind_zm, int ad_pocz, int ad_kon, int ind_zm_dod) {
	instrukcja instr;

	instr.polecenie = pol;
	instr.indeks_zmiennej = ind_zm;
	instr.adres_pocz = ad_pocz;
	instr.adres_kon = ad_kon;
	instr.indeks_zmiennej_dod = ind_zm_dod;

	return instr;
}

//Procedura zwiększająca rozmiar talicy instrukcji w przypadku potrzeby
void reall(kod* maszynowy, int ind) {
	int akt_r = obecny_rozm(sizeof(instrukcja), ind);
	if ( akt_r >= maszynowy->rozm) {
		int rozmiarek = nowy_rozm(maszynowy->rozm, sizeof(instrukcja));
		maszynowy->rozm = rozmiarek;
		instrukcja* t = realloc(maszynowy->instr, (size_t)maszynowy->rozm);
		if (t == NULL) printf("MIAŁO BYĆ BEZ NULLA\n");
		else maszynowy->instr = t;
	}
}

//Procedura optymalizująca kod pętli Powtarzaj na podstawie tablicy instrukcji
//oraz instrukcji DJZ. Nadpisuje uzupełnione wcześniej komórki tablicy instrukcji
void optymalizacja(instrukcja** instr, instrukcja poczatek) {
	int i = poczatek.adres_pocz + 1; //(a
	instrukcja pol;
	while ((*instr)[i].polecenie != JMP) {
		pol = stworz_instr(ADD, (*instr)[i+1].indeks_zmiennej, -1, -1, poczatek.indeks_zmiennej); //(ab
		(*instr)[i++] = pol;  //(a|b przechodzi na b
	}
	pol = stworz_instr(CLR, poczatek.indeks_zmiennej, -1, -1, -1);
	(*instr)[i-1] = pol;
}


//Funkcja inicjalizująca początek instrukcji Powtarzaj. Nadaje odpowiednie wartości
//instrukcjom ADD i DJZ, umieszcza w tablicy instrukcji struktury kod, aktualizuje
//indeks w tablicy ostatniej wolnej komórki w tablicy
int djzadd(kod* maszynowy, int* ind) {
	instrukcja pol = stworz_instr(DJZ, -1, *ind, -1, -1);
	reall(maszynowy,*ind);
	maszynowy->instr[(*ind)] = pol;
	int zm_ind = zmienna();
	maszynowy->instr[(*ind)++].indeks_zmiennej = zm_ind;
	pol = stworz_instr(ADD, zm_ind, -1, -1, ODJ); //(a określa a
	reall(maszynowy, *ind);
	maszynowy->instr[(*ind)++] = pol;
	return zm_ind;
}

/*Procedura uzupełniająca tablicę instrukcji struktury kod we fragmentach
zawierających pętlę powtarzaj. Wyszukuje zamknięcia nawiasów za pomocą wskaźników,
do pierwszej nieuzupełnionej instrukcji DJZ, by nie alokować nowej pamięci.
Aktualizuje indeks ostatniej wolnej komórki w tablicy */
void wpisz_powtarzaj(kod* maszynowy, int* ind) {
	instrukcja pol;

	int nawias = 1;
	int j; //kontroluje do tyłu
	int g;  //getchar()
	int takie_same = 0;  //Liczba powtarzających się zmiennych
	bool mozna = true;   //Jeśli pętla nie zawiera pętli, można optymalizować

	int poczatkowy_char = djzadd(maszynowy, ind);  //Pierwsza zmienna po '('
	while (nawias != 0) {
		g = getchar();
		int przypis = przelicz_indeks(g);
		if (przypis >=0 && przypis <= LITERY) {
			pol = stworz_instr(INC, przypis, -1, -1, DOD);
			reall(maszynowy, *ind);
			maszynowy->instr[(*ind)++] = pol;
		} else if (g == '(') {
			nawias++;
			poczatkowy_char = djzadd(maszynowy, ind); //Sprawdzamy tylko tę najbardziej wewnetrzna
		} else if (g == ')') {  //rozpoczyna przeszukiwanie do tyłu
			nawias--;
			j = (*ind)-1;
			while (!(maszynowy->instr[j].polecenie == DJZ && maszynowy->instr[j].adres_kon == -1)) {
				if (maszynowy->instr[j].polecenie == JMP) mozna = false;   //Jeśli zagnieżdżona pętla
				if (maszynowy->instr[j].indeks_zmiennej == poczatkowy_char) {
					takie_same++;
				}
				j--;
			}
			maszynowy->instr[j].adres_kon = (*ind) + 1; //Adres kończowy to pierwsza pozycja po ')'
			pol = stworz_instr(JMP, -1, -1, maszynowy->instr[j].adres_pocz, -1); //dokad skacze
			reall(maszynowy, *ind);
			maszynowy->instr[(*ind)++] = pol;
			if (mozna && takie_same == 1) {
				optymalizacja(&(maszynowy->instr), maszynowy->instr[j]);
			}
			mozna = true;
			takie_same = 0;
		}
	}
}

//Procedura uzupełniająca tablicę instrukcji struktury kod na podstawie inputu,
//podanego również jako znak g; aktualizuje indeks ostatniej wolnej komórki
//w tablicy
void maszyna(kod* maszynowy, int g, int* i) {
	instrukcja pol;

	while (g != '\n') {   //Do końca programu
		reall(maszynowy, *i);
		int indeks = przelicz_indeks(g);
		if (indeks >= 0 && indeks <= LITERY) {   //Jeśli polecenie INC
			pol = stworz_instr(INC, indeks, -1, -1, DOD);
			maszynowy->instr[(*i)++] = pol;
		} else {
			wpisz_powtarzaj(maszynowy, i); //zmienia wartosc i wewnatrz funkcji;
		}
		g = getchar();
	}

	pol = stworz_instr(HLT, -1, -1, -1, -1);
	reall(maszynowy, *i);
	maszynowy->instr[*i] = pol;
}


//Procedura wypisująca liczbę w systemie o podstawie 10000 jako liczbę w sysytemie
//dziesiętnym
void wypisz_liczbe(liczba num) {
	if (num.ile_kom == 1) {
		printf("%d", num.komorka[0]);
	} else {
		for (int i = num.ile_kom - 1; i>=0; i--) {
			if (i == num.ile_kom - 1) {
				printf("%d", num.komorka[i]);
			} else {
				printf("%0*d", 4, num.komorka[i]);
			}
		}
	}
	printf("\n");
}

//Procedura zerująca daną liczbę; zmniejszająca rozmiar oraz aktualizujące
//pozostałe elementy struktury liczba
void wyczysc(liczba* a) {
	a->komorka = realloc(a->komorka, ILE_CYFR*sizeof(int));
	a->komorka[0] = 0;
	a->ile_kom = 1;
	a->rozmiar = ILE_CYFR*sizeof(int);
}


/*Procedura realizująca instrukcję DZJ. Jeśli wartość zmiennej następującej po '('
wynosi zero, przesuwa indeks iterujący po tablicy instrukcji na pierszy indeks
po zamknięciu pętli. Jeżeli pierwsza zmienna jest niezerowa, doprowadza do odjęcia
liczby poprzez przekierowanie do instrukcji odpowiadającej za odejmowanie
*/
void dec_or_jump(instrukcja pol, liczba** licznik, int* akt_ind) {
	liczba zmienna = (*licznik)[pol.indeks_zmiennej];
	if (zmienna.komorka[0] == 0 && zmienna.ile_kom == 1) { //Jeśli zerowa
		(*akt_ind) = pol.adres_kon; //do nastepnej instrukcji
	} else {
		(*akt_ind)++;
	}
}

/* Procedura interpretująca kod maszynowy oraz aktualizująca tablicę zmiennych
według kolejnych instrukcji od początku do końca tablicy kodu maszynowego (*ind).
Podczas optymalizacji kodu pozostawiane są nawiasy, by w przypadku zagnieżdżenia
pętli i ponownej iteracji przejść do pierwszej instrukcji po zoptymalizowanej
pętli dzięki DJZ zamiast dodawać zero do ciągu zmiennych, ujętych wcześniej
w pętli Powtarzaj. Dlatego uwzględniono instrukcję ADD ze zmienną -1, by zachować
miejsce dla CLR w przypadku optymalizacji.
*/
void interpreter(kod* maszynowy, liczba** licznik, int* ind) {
	int i = 0;
	instrukcja pol;
	while (i <= (*ind)) {
		pol = maszynowy->instr[i];
		switch(pol.polecenie) {
			case ADD:
				if (pol.indeks_zmiennej_dod == ODJ) {
					odejmij((*licznik + pol.indeks_zmiennej));
					i++;
				} else {
					add((*licznik + pol.indeks_zmiennej), (*licznik + pol.indeks_zmiennej_dod));
					i++;
				}
				break;
			case INC:
				add((*licznik + pol.indeks_zmiennej), (*licznik + DOD));
				i++;
				break;
			case DJZ:
				dec_or_jump(pol, licznik, &i);
				break;
			case  JMP:
				i = pol.adres_kon;
				break;
			case CLR:
				wyczysc((*licznik + pol.indeks_zmiennej));
				i++;
				break;
			case HLT:
				*ind = 0;
				maszynowy->rozm = 0;
				free(maszynowy->instr);
				maszynowy->instr = NULL;
				return;
		}
	}
}

//Procedura wyświetlająca wartości zmiennych, pobierająca input, aktualizująca
//tablicę instrukcji oraz zmieniająca wartości zmiennych do zakończenia pliku
void rozgrywka(liczba** licznik, kod* maszynowy) {
	int indeks = 0;

	int g;
	while ((g = getchar()) != EOF) {
		if (g == '=') {
			int ind = zmienna();
			wypisz_liczbe((*licznik)[ind]);
			getchar();
		} else {
			maszyna(maszynowy, g, &indeks);
			interpreter(maszynowy, licznik, &indeks);
		}
	}
}

//Procedura zwalniająca pamięć zajmowaną przez tablicę instrukcji
void zwolnij_instr(instrukcja** instr) {
	free(*instr);
	*instr = NULL;
}

//Procedura zwalniająca pamięć zajmowaną przez tablicę zmiennych
void zwolnij_licz(liczba** licznik) {
	for (int i = 0; i< DOD + 1; i++) {
		free((*licznik)[i].komorka);
	}
	free(*licznik);
}

//Procedura inicjalizują wartości struktury kod
void inicjalizuj_kod(kod* maszynowy) {
	maszynowy->instr = NULL;
	maszynowy->rozm = 0;
}

int main(void) {
	liczba* zmienne = NULL;
	stworz_zmienne(&zmienne);
	kod maszynkowy;
	inicjalizuj_kod(&maszynkowy);

	rozgrywka(&zmienne, &maszynkowy);
	zwolnij_instr(&(maszynkowy.instr));
	zwolnij_licz(&zmienne);
	return 0;
}
