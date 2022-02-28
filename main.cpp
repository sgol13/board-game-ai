#include <ctime>
#include <iostream>
#include <queue>
#include <stdlib.h>
#include <unistd.h>
#include <utility>

using namespace std;

// wyswietla aktualny stan planszy
void wyswietlPlansze(const int p[8][8]) {

    cout << "\033[2J\033[1;1H";
    cout << "\n\n";

    for (int y = 7; y >= 0; y--) {
        cout << "    " << (char)('A' + y) << " |";
        for (int x = 0; x <= 7; x++) {

            char znak = ' ';
            if (p[x][y] == 1)
                znak = '#';
            else if (p[x][y] == 2)
                znak = 'B';
            else if (p[x][y] == 3)
                znak = 'C';

            cout << znak << "|";
        }
        cout << endl;
    }

    cout << "       ";
    for (int x = 0; x <= 7; x++)
        cout << x << " ";
    cout << endl;
}

// tworzy nowa plansze
void nowaPlansza(int p[8][8]) {

    // czarne pola
    p[0][3] = p[1][1] = p[1][6] = p[2][4] = p[3][2] = p[3][7] = 1;
    p[4][0] = p[4][5] = p[5][3] = p[6][1] = p[6][6] = p[7][4] = 1;

    p[5][0] = p[6][0] = p[7][0] = 2; // piony B
    p[0][7] = p[1][7] = p[2][7] = 3; // piony C
}

// zmienia oznaczenie wiersza / kolumny na liczbe
int zmienNaLiczbe(char x) {

    if (x >= 'a' && x <= 'h') // litera 'a' - 'h'
        return x - 'a';
    else if (x >= '0' && x <= '7') // cyfra '0' - '7'
        return x - '0';
    else
        return 0; // blad jesli cos innego
}

// sprawdza czy nr wiersza / kolumny jest w odpowiednim zakresie
bool sprawdzZakres(int x) { return x >= 0 && x <= 7; }

// wczytuje ruch, zwraca true jesli zostal poprawnie wczytany, a false jesli nie
bool wczytajRuch(int ruch[4]) {

    string wejscie;
    getline(cin, wejscie); // wczytaj wiersz wejscia

    // omin biale znaki, jesli jakies sa
    int r = 0;
    while (wejscie[r] == '\t' || wejscie[r] == ' ')
        ++r;

    bool poprawny_format = true;

    // wiersz poczatkowy
    if (isalpha(wejscie[r]) && sprawdzZakres(zmienNaLiczbe(wejscie[r])))
        ruch[1] = zmienNaLiczbe(wejscie[r++]);
    else
        poprawny_format = false;

    // kolumna poczatkowa
    if (isdigit(wejscie[r]) && sprawdzZakres(zmienNaLiczbe(wejscie[r])))
        ruch[0] = zmienNaLiczbe(wejscie[r++]);
    else
        poprawny_format = false;

    // omin biale znaki posrodku
    while (wejscie[r] == '\t' || wejscie[r] == ' ')
        ++r;

    // wiersz koncowy
    if (isalpha(wejscie[r]) && sprawdzZakres(zmienNaLiczbe(wejscie[r])))
        ruch[3] = zmienNaLiczbe(wejscie[r++]);
    else
        poprawny_format = false;

    // kolumna koncowa
    if (isdigit(wejscie[r]) && sprawdzZakres(zmienNaLiczbe(wejscie[r])))
        ruch[2] = zmienNaLiczbe(wejscie[r++]);
    else
        poprawny_format = false;

    // jesli jest cos poza tym, wejscie jest niepoprawne
    while (wejscie[r]) {
        if (wejscie[r] != ' ' && wejscie[r] != '\t' && wejscie[r] != '\n')
            poprawny_format = false;
        ++r;
    }

    return poprawny_format;
}

// zwraca true jesli ruch jest mozliwy do wykonania, false jesli nie
bool mozliwyRuch(const int ruch[4], int p[8][8]) {

    int x1 = ruch[0];
    int y1 = ruch[1];
    int x2 = ruch[2];
    int y2 = ruch[3];
    int dx = 0, dy = 0;

    if (x1 == x2 && y1 == y2) // sprawdz czy ruch nie jest pusty
        return false;

    if (p[x1][y1] != 2) // sprawdz czy pionek jest w tym miejscu
        return false;

    if (y1 == y2) { // wiersz
        if (x1 < x2)
            dx = 1;
        else
            dx = -1;
    }

    if (x1 == x2) { // kolumna
        if (y1 < y2)
            dy = 1;
        else
            dy = -1;
    }

    if (x1 + y1 == x2 + y2) { // przekatna '\'
        if (x1 < x2) {
            dx = 1;
            dy = -1;
        } else {
            dx = -1;
            dy = 1;
        }
    }

    if (x1 - y1 == x2 - y2) { // przekatna '/'
        if (x1 < x2)
            dx = dy = 1;
        else
            dx = dy = -1;
    }

    if (dx == 0 && dy == 0) // zaden ruch nie zostal znaleziony
        return false;

    // sprawdz czy nie ma przeszkod
    do {
        x1 += dx;
        y1 += dy;

        if (p[x1][y1] != 0)
            return false;

    } while (x1 != x2 || y1 != y2);

    return true;
}

// aktualizuje pozycje pionow
void wykonajRuch(const int ruch[4], int piony[3][2], int p[8][8], const int gracz) {

    p[ruch[0]][ruch[1]] = 0;
    p[ruch[2]][ruch[3]] = gracz;

    for (int i = 0; i < 3; i++)
        if (ruch[0] == piony[i][0] && ruch[1] == piony[i][1]) {
            piony[i][0] = ruch[2];
            piony[i][1] = ruch[3];
            break;
        }
}

// zwraca wartosc oceniajaca dany stan planszy wzgledem podanego gracza
int ocenStan(const int p[8][8], const int piony[3][2], const int gracz) {

    int wartosc_stanu = 0;
    const int kierunek[8][2] = {{0, 1}, {1, 0},  {0, -1},  {-1, 0},
                                {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
    const int meta[2][3][2] = {{{0, 7}, {1, 7}, {2, 7}}, {{5, 0}, {6, 0}, {7, 0}}};

    // dla kazdego pionka sprawdza w ile ruchow moze dotrzec do celu
    for (int pion = 0; pion < 3; pion++) {

        // algorytm BFS
        int d[8][8]{};

        int pionek_x = piony[pion][0];
        int pionek_y = piony[pion][1];

        queue<pair<int, int>> kolejka;
        kolejka.push(make_pair(pionek_x, pionek_y));
        d[pionek_x][pionek_y] = 1;

        while (!kolejka.empty()) {

            // bierze pierwszy element z kolejki
            pionek_x = kolejka.front().first;
            pionek_y = kolejka.front().second;
            kolejka.pop();

            // dodaje do kolejki wszystkie mozliwe ruchy tego pionka
            for (int nr_kierunku = 0; nr_kierunku < 8; nr_kierunku++) {

                int x = pionek_x;
                int y = pionek_y;
                int dx = kierunek[nr_kierunku][0];
                int dy = kierunek[nr_kierunku][1];
                while (sprawdzZakres(x + dx) && sprawdzZakres(y + dy) &&
                       (p[x + dx][y + dy] == 0 || p[x + dx][y + dy] == gracz)) {
                    x += dx;
                    y += dy;

                    if (d[x][y] == 0) {
                        d[x][y] = d[pionek_x][pionek_y] + 1;
                        kolejka.push(make_pair(x, y));
                    }
                }
            }
        }

        // oblicz dystans do najblizszego pola koncowego
        int najmniejszy_dystans = 10;

        for (int i = 0; i < 3; i++) {

            int meta_x = meta[gracz - 2][i][0];
            int meta_y = meta[gracz - 2][i][1];

            if (piony[pion][0] == meta_x && piony[pion][1] == meta_y)
                wartosc_stanu -= 100;

            if (d[meta_x][meta_y] != 0 && d[meta_x][meta_y] < najmniejszy_dystans)
                najmniejszy_dystans = d[meta_x][meta_y];
        }

        // powieksz wartosc obecnego stanu
        wartosc_stanu += najmniejszy_dystans;
    }

    return wartosc_stanu;
}

// znajduje optymalny ruch dla komputera
void ruchKomputera(int ruch[4], int p[8][8], int czarne[3][2], int biale[3][2]) {

    int minimalna_wartosc = 1000;
    const int kierunek[8][2] = {{0, 1}, {1, 0},  {0, -1},  {-1, 0},
                                {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
    int znalezione_ruchy[100][4];
    int liczba_ruchow = 0;

    // sprawdz mozliwe ruchy we wszystkich kierunkach dla kazdego pionka
    for (int pion = 0; pion < 3; pion++) {
        for (int nr_kierunku = 0; nr_kierunku < 8; nr_kierunku++) {

            int x = czarne[pion][0];
            int y = czarne[pion][1];
            const int poprzednie_x = x;
            const int poprzednie_y = y;
            int dx = kierunek[nr_kierunku][0];
            int dy = kierunek[nr_kierunku][1];

            // wykonuje kolejne ruchy w danym kierunku
            while (sprawdzZakres(x + dx) && sprawdzZakres(y + dy) &&
                   p[x + dx][y + dy] == 0) {

                x += dx;
                y += dy;

                // wykonaj ruch
                ruch[0] = poprzednie_x;
                ruch[1] = poprzednie_y;
                ruch[2] = x;
                ruch[3] = y;

                wykonajRuch(ruch, czarne, p, 3);

                // ocen stan planszy po wykonaniu danego ruchu
                int wartosc = ocenStan(p, czarne, 3);

                // zapisz nowy najlepszy ruch lub kolejny o najlepszej wartosci
                if (wartosc < minimalna_wartosc) {

                    minimalna_wartosc = wartosc;
                    liczba_ruchow = 1;
                    for (int i = 0; i < 4; i++)
                        znalezione_ruchy[0][i] = ruch[i];

                } else if (wartosc == minimalna_wartosc) {

                    // dopisz kolejny ruch o tej samej wartosci
                    for (int i = 0; i < 4; i++)
                        znalezione_ruchy[liczba_ruchow][i] = ruch[i];
                    liczba_ruchow++;
                }

                // wycofaj ruch
                ruch[0] = x;
                ruch[1] = y;
                ruch[2] = poprzednie_x;
                ruch[3] = poprzednie_y;

                wykonajRuch(ruch, czarne, p, 3);
            }
        }
    }

    // jesli najlepszych ruchow jest wiele, wybierz losowo
    int nr_ruchu = rand() % liczba_ruchow;
    for (int i = 0; i < 4; i++)
        ruch[i] = znalezione_ruchy[nr_ruchu][i];
}


// zwraca nr gracza jesli wygral, 0, jesli nie
int sprawdzWygrana(const int p[8][8]) {

    if (p[0][7] == 2 && p[1][7] == 2 && p[2][7] == 2)
        return 2;

    if (p[5][0] == 3 && p[6][0] == 3 && p[7][0] == 3)
        return 3;

    return 0;
}

int main() {

    srand(time(NULL));

    int p[8][8]{}; // plansza
    int ruch[4];
    int czarne[3][2] = {{0, 7}, {1, 7}, {2, 7}}; // aktualne pozycje czarnych pionow
    int biale[3][2] = {{5, 0}, {6, 0}, {7, 0}};
    nowaPlansza(p);

    int zwyciezca = 0;

    while (zwyciezca == 0) {

        wyswietlPlansze(p);

        // wczytywanie ruchu gracza
        bool poprawny_ruch = 0;

        while (poprawny_ruch == 0) {

            wyswietlPlansze(p);
            cout << "\n   (bialy) ";

            poprawny_ruch = wczytajRuch(ruch);
            if (poprawny_ruch)
                poprawny_ruch = mozliwyRuch(ruch, p);
        }

        wykonajRuch(ruch, biale, p, 2);

        // ruch komputera
        wyswietlPlansze(p);
        cout << "\n   (czarny) " << endl;
        sleep(1);

        ruchKomputera(ruch, p, czarne, biale);
        wykonajRuch(ruch, czarne, p, 3);

        zwyciezca = sprawdzWygrana(p);
    }

    wyswietlPlansze(p);

    if (zwyciezca == 2)
        cout << "\n  ZWYCIESTWO\n\n";
    else
        cout << "\n  PORAZKA\n\n";

    return 0;
}