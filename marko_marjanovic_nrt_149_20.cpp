/*
 * Lisice VS Zecevi
 * 
 * Definicaja sveta:
 * Na svetu postoje dve vrste zivotinja: lisice i zecevi. Pored zivotinja, postoje i "zbunovi".
 * Velicinu sveta kao i broj zivotinja unosi korisnik prilikom pokretanja programa. 
 * 
 * Lisice i zecevi nasumicno idu po svetu. Kada zec upadne u vidokrug lisice ona se krece
 * pravo prema zecu. Kada lisica upadne u vidokrug zeca ako vidi zbun on bezi ka njemu
 * gde zec moze ostati sakriven odredjeni broj krugova. Lisica nastavlja da se krece 
 * dalje nasumicno po svetu. 
 * 
 */

#include <iostream>
#include <unistd.h>
#include <math.h>
#include <iomanip>
#include <fstream>

using namespace std;

#define MIN_VELICINA_SVETA 10
#define MAX_VELICINA_SVETA 200
#define MAX_BROJ_CIKLUSA 10000
#define MAX_BROJ_LISICA 20
#define MAX_BROJ_ZECEVA 20
#define MAX_BROJ_ZBUNOVA 20
#define VIDOKRUG_LISICE 10
#define VIDOKRUG_ZECA 6
#define ZEC_U_ZBUNU_CIKLUSI 5
#define BRZINA_LISICE 1
#define BRZINA_ZECA 1

#define LISICA "\e[97;41mL\e[0m"
#define ZEC "\e[30;46mZ\e[0m"
#define VIDOKRUG "\e[0;33m+\e[0m"
#define ZBUN "\e[32;42m \e[0m"
#define ZEC_U_ZBUNU "\e[30;42mZ\e[0m"
#define POZADINA "\e[10;48m \e[0m"

void unosParametara();
void simulirajSvet();
void racunanjeSveta();
void inicijalizacijaObjekata();
int xyUSvet(int x, int y);
bool uRadijusu(int x1, int y1, int x2, int y2, int razdaljina);
void upisiRezultateUFajl();

int velicinaSvetaX = 50, velicinaSvetaY = 30, *svet, brojCiklusa = 100;
int brojLisica = 2, brojZeceva = 10, brojZbunova = 5, brojSmrti = 0;

class Zivotinja;
class Lisica;
class Zec;
class Zbun;

Zivotinja *lisice;
Zivotinja *zecevi;
Zbun *zbunovi;

/**
 * @brief Klasa za Zbun
 * Ima X i Y poziciju kojom se generise prikaz na ekranu
 */
class Zbun
{
public:
    int x, y;

    Zbun()
    {
        x = 1 + (rand() % (velicinaSvetaX));
        y = 1 + (rand() % (velicinaSvetaY));
    }

    /**
     * @brief Funkcija koja se poziva prilikom svakog ciklusa simulacije
     */
    void ciklus()
    {
        svet[xyUSvet(x, y)] = 4;
    }
};

/**
 * @brief Osnovna klasa za sve zivotinje
 * Klasa zivotinje sadrzi osnovne funkcionalnosti koje sve zivotinje u 
 * simulaciji imaju zajednicko.
 */
class Zivotinja
{
public:
    int brzinaPomeranja, x, y, tipZivotinje, tipProtivnika, vidokrug;
    int zivotinjaUVidikuX, zivotinjaUVidikuY, zbunUVidokruguX, zbunUVidokruguY;
    bool ziv;

    Zivotinja() {}

    /**
     * @brief Konstrukt za novu Zibotinju
     * Inicijalizuje pocetno stanje zivotinje, i postavlja je na svet
     * 
     * @param brzinaPomeranjaInit - brzina zivotinje 
     */
    explicit Zivotinja(int brzinaPomeranjaInit)
    {
        brzinaPomeranja = brzinaPomeranjaInit;
        x = 1 + (rand() % (velicinaSvetaX - 1));
        y = 1 + (rand() % (velicinaSvetaY - 1));
        zivotinjaUVidikuX = 0;
        zivotinjaUVidikuY = 0;
        ziv = true;

        postaviNaSvet();
    }

    /**
     * @brief Funkcija koja se poziva prilikom svakog ciklusa simulacije
     */
    void ciklus()
    {
        novaPozicija();
        postaviNaSvet();
    }

    /**
     * @brief Kalkulacija nove pozicije.
     * Kako svaka zivotinja ima svoje izracunavanje pozicije, ova funkcija je 
     * virtuelna 
     */
    virtual void novaPozicija() {}

    /**
     * @brief Postavljanje zivotinje u array sveta
     * Ukoliko je zivotinja u zivotu, predstavljamo je na svetu tako sto dodajemo
     * njen broj u array svet kao i njeno vidno polje
     */
    void postaviNaSvet()
    {
        if (ziv)
        {
            // Postavljamo vidokrug
            int i, j, pozicijaNaSvetu;
            for (i = y - vidokrug; i < y + vidokrug; i++)
            {
                for (j = x - vidokrug; j < x + vidokrug; j++)
                {
                    // Test da li je tacka u okviru sveta, test razdaljine i
                    // test da li je prazno polje
                    pozicijaNaSvetu = xyUSvet(j, i);
                    if (i > 0 && j > 0 && i <= velicinaSvetaY && j <= velicinaSvetaX && uRadijusu(x, y, j, i, vidokrug) && svet[pozicijaNaSvetu] == 0)
                    {
                        svet[pozicijaNaSvetu] = 3;
                    }
                }
            }

            // Postavljamo zivotinju u array sveta
            svet[xyUSvet(x, y)] = tipZivotinje;
        }
    }

    /**
     * @brief Da li postoji neka zivotinja u vidnom polju
     * 
     * @param zivotinje 
     * @param brojZivotinja 
     * @return bool 
     * 
     */
    bool daLiJeZivotinjaUVidokrugu(Zivotinja *zivotinje, int brojZivotinja)
    {
        int i;
        // Ako je zivotinja u vidokrugu, ziva i ako nije u zbunu, setujemo X i Y
        // protivnika i izlazimo iz funkcije
        for (i = 0; i < brojZivotinja; i++)
        {
            if (zivotinje[i].ziv && uRadijusu(x, y, zivotinje[i].x, zivotinje[i].y, vidokrug) && svet[xyUSvet(zivotinje[i].x, zivotinje[i].y)] != 5)
            {
                zivotinjaUVidikuX = zivotinje[i].x;
                zivotinjaUVidikuY = zivotinje[i].y;
                return true;
            };
        }
        return false;
    }

    /**
     * @brief Da li je neki zbun u vidokrugu zivotinje
     * 
     * @return bool 
     */
    bool daLiJeZbunUVidokrugu()
    {
        int i, noviVidokrug = vidokrug;
        // Ako je zbun u vidokrugu, setujemo X i Y zbuna i izlazimo iz funkcije
        for (i = 0; i < brojZbunova; i++)
        {
            if (uRadijusu(x, y, zbunovi[i].x, zbunovi[i].y, vidokrug))
            {
                zbunUVidokruguX = zbunovi[i].x;
                zbunUVidokruguY = zbunovi[i].y;
                return true;
            };
        }
        return false;
    }
};

/**
 * @brief Lisica
 * Klasa definise specificnosti jedne Lisice
 */
class Lisica : public Zivotinja
{
public:
    Lisica() {}

    /**
     * @brief Konstruktor Lisice
     * 
     * @param brzinaPomeranjaInit 
     * Funkcija prima parametar za odredjivanje brzine lisice i setuje osnovne parametre
     */
    explicit Lisica(int brzinaPomeranjaInit) : Zivotinja(brzinaPomeranjaInit)
    {
        tipZivotinje = 1;
        vidokrug = VIDOKRUG_LISICE;
    };

    /**
     * @brief Racunanje nove pozicije lisice tokom svakog ciklusa
     */
    void novaPozicija()
    {
        int noviX = 0, noviY = 0;

        // Ako je zec u vidokrugu idi ka njemu
        if (daLiJeZivotinjaUVidokrugu(zecevi, brojZeceva) == true)
        {
            // Izracunaj ugao izmedju zeca i sebe, i dodaj 180 stepeni
            float ugao = atan2(y - zivotinjaUVidikuY, x - zivotinjaUVidikuX) + 3.14;
            // Pomeri lisicu ka zecu
            noviX = round(x + brzinaPomeranja * cos(ugao));
            noviY = round(y + brzinaPomeranja * sin(ugao));
        }
        else
        {
            // Ako nema zeca u vidokrugu, lutaj po svetu
            noviX = x + ((rand() % (brzinaPomeranja * 2 + 1)) - brzinaPomeranja);
            noviY = y + ((rand() % (brzinaPomeranja * 2 + 1)) - brzinaPomeranja);
        }

        // Ako neka dimezija izlazi van okivra sveta preskacemo je
        if (noviX > 0 && noviX <= velicinaSvetaX)
            x = noviX;
        if (noviY > 0 && noviY <= velicinaSvetaY)
            y = noviY;
    }
};

/**
 * @brief Zec
 * Klasa definise specificnosti jednog Zeca
 */
class Zec : public Zivotinja
{
public:
    int uZbunu;
    Zec() {}

    /**
     * @brief Konstruktor Zeca
     * 
     * @param brzinaPomeranjaInit 
     * Funkcija prima parametar za odredjivanje brzine zeca i setuje osnovne parametre
     */
    explicit Zec(int brzinaPomeranjaInit) : Zivotinja(brzinaPomeranjaInit)
    {
        tipZivotinje = 2;
        vidokrug = VIDOKRUG_ZECA;
        uZbunu = ZEC_U_ZBUNU_CIKLUSI;
    };

    /**
     * @brief Racunanje nove pozicije zeca tokom svakog ciklusa
     */
    void novaPozicija()
    {
        int noviX = 0, noviY = 0;

        // Ako je zec u zbunu predugo, mora da izadje
        if (uZbunu == 0)
        {
            uZbunu = ZEC_U_ZBUNU_CIKLUSI;
            tipZivotinje = 2;
        }

        // Ako je u zbunu smanjujemo broj ciklusa koliko jos moze da bude unutra
        // i izlazimo iz funkcije (zec ostaje na istom mestu)
        if (uZbunu != ZEC_U_ZBUNU_CIKLUSI)
        {
            uZbunu--;
            return;
        }
        else if (daLiJeZivotinjaUVidokrugu(lisice, brojLisica) == true)
        {
            // Ako ima lisica u vidiku
            // Ako je lisica na istoj poziciji, zec gubi zivot
            // i inkrementiramo broj smrti
            if (x == zivotinjaUVidikuX && y == zivotinjaUVidikuY)
            {
                brojSmrti++;
                ziv = false;
                return;
            }
            // Ako je zbun u vidokrugu, idi ka njemu
            if (daLiJeZbunUVidokrugu())
            {
                float ugao = atan2(y - zbunUVidokruguY, x - zbunUVidokruguX) + 3.14;
                noviX = round(x + brzinaPomeranja * cos(ugao));
                noviY = round(y + brzinaPomeranja * sin(ugao));

                // Da li je zec usao u zbun
                if (noviX == zbunUVidokruguX && noviY == zbunUVidokruguY)
                {
                    // Smanji broj ciklusa
                    uZbunu--;
                    // Setuj tip zivotinje za iscrtavanje (zec u zbunu)
                    tipZivotinje = 5;
                    // Posto je zbun svakako u svetu, nije potrebna provera sa dna
                    // funkcije, pa setujemo novi X i Y, i izlazimo
                    x = noviX;
                    y = noviY;
                    return;
                }
            }
            else
            {
                // Ako nema zbuna na vidiku, bezi od lisice na drugu stranu
                float ugao = atan2(y - zivotinjaUVidikuY, x - zivotinjaUVidikuX);
                noviX = round(x + brzinaPomeranja * cos(ugao));
                noviY = round(y + brzinaPomeranja * sin(ugao));
            }
        }
        else
        {
            // Lutaj po mapi ako nema opasnosti
            noviX = x + ((rand() % (brzinaPomeranja * 2 + 1)) - brzinaPomeranja);
            noviY = y + ((rand() % (brzinaPomeranja * 2 + 1)) - brzinaPomeranja);
        }

        // Ako neka dimezija izlazi van okivra sveta preskacemo je
        if (noviX > 0 && noviX <= velicinaSvetaX)
        {
            x = noviX;
        }
        if (noviY > 0 && noviY <= velicinaSvetaY)
        {
            y = noviY;
        }
    }
};

/**
 * @brief Unos parametara za simulaciju od strane korisnika
 */
void unosParametara()
{
    do
    {
        cout << "Unesite X velicinu sveta (od " << MIN_VELICINA_SVETA << " do " << MAX_VELICINA_SVETA << ", preporuka 150): ";
        cin >> velicinaSvetaX;
    } while (MIN_VELICINA_SVETA > velicinaSvetaX && velicinaSvetaX < MAX_VELICINA_SVETA);

    do
    {
        cout << "Unesite Y velicinu sveta (od " << MIN_VELICINA_SVETA << " do " << MAX_VELICINA_SVETA << ", preporuka 50): ";
        cin >> velicinaSvetaY;
    } while (MIN_VELICINA_SVETA > velicinaSvetaY && velicinaSvetaY < MAX_VELICINA_SVETA);

    do
    {
        cout << "Unesite broj lisica (maksimum " << MAX_BROJ_LISICA << "): ";
        cin >> brojLisica;
    } while (0 > brojLisica && brojLisica < MAX_BROJ_LISICA);

    do
    {
        cout << "Unesite broj zeceva (maksimum " << MAX_BROJ_ZECEVA << "): ";
        cin >> brojZeceva;
    } while (0 > brojZeceva && brojZeceva < MAX_BROJ_ZECEVA);

    do
    {
        cout << "Unesite broj zbunova (maksimum " << MAX_BROJ_ZBUNOVA << "): ";
        cin >> brojZbunova;
    } while (0 > brojZbunova && brojZbunova < MAX_BROJ_ZBUNOVA);

    do
    {
        cout << "Unesite broj ciklusa koje zelite da simulirate (maksimum " << MAX_BROJ_CIKLUSA << "): ";
        cin >> brojCiklusa;
    } while (0 > velicinaSvetaY && velicinaSvetaY < MAX_BROJ_CIKLUSA);
}

/**
 * @brief Pocetna inicijalizacija objekata na svetu
 * 
 */
void inicijalizacijaObjekata()
{
    int i;

    // Init zbunova
    for (i = 0; i < brojZbunova; i++)
    {
        zbunovi[i] = Zbun();
    }

    // Init lisica
    for (i = 0; i < brojLisica; i++)
    {
        lisice[i] = Lisica(BRZINA_LISICE);
    }

    // Init zeceva
    for (i = 0; i < brojZeceva; i++)
    {
        zecevi[i] = Zec(BRZINA_ZECA);
    }
}

/**
 * @brief Simulacija sveta u zadanom broju ciklusa
 * 
 */
void simulirajSvet()
{
    int i;
    int noviRed = 0;
    int k;

    for (k = 0; k < brojCiklusa; k++)
    {
        // Izracunaj pozicije za ciklus
        racunanjeSveta();

        // CRTANJE SVETA
        // Brisem ekran
        printf("\033c");

        // Iscrtavam (1-lisice, 2-zec, 3-vidokrug, 4-zbun, 5-zec u zbunu)
        for (i = 0; i < velicinaSvetaY * velicinaSvetaX; i++)
        {
            switch (svet[i])
            {
            case 1:
                cout << LISICA;
                break;
            case 2:
                cout << ZEC;
                break;
            case 3:
                cout << VIDOKRUG;
                break;
            case 4:
                cout << ZBUN;
                break;
            case 5:
                cout << ZEC_U_ZBUNU;
                break;
            default:
                cout << POZADINA;
                break;
            }

            // Novi red na kraju X ose
            noviRed++;
            if (noviRed == velicinaSvetaX)
            {
                cout << "\n";
                noviRed = 0;
            }
        }
        // Pauza u izvrsavanju
        usleep(300000);
    }
}

/**
 * @brief Racunanje parametara za svaki cuklus
 */
void racunanjeSveta()
{
    int i;

    // Resetujemo sva polja na 0
    for (i = 0; i < velicinaSvetaY * velicinaSvetaX; i++)
    {
        svet[i] = 0;
    }

    // Postavi sve zbunove na mapu
    for (i = 0; i < brojZbunova; i++)
    {
        zbunovi[i].ciklus();
    }

    // Izracunaj pozicije svih zeceva
    for (i = 0; i < brojZeceva; i++)
    {
        zecevi[i].ciklus();
    }

    // Izracunaj pozicije svih lisica
    for (i = 0; i < brojLisica; i++)
    {
        lisice[i].ciklus();
    }
}

/**
 * @brief Odredjuje da li je je razdaljina izmedju 2 tacke manja od zadate
 * 
 * @param x1 
 * @param y1 
 * @param x2 
 * @param y2 
 * @param razdaljina 
 * @return bool 
 */
bool uRadijusu(int x1, int y1, int x2, int y2, int razdaljina)
{
    double r = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    if (round(r) < razdaljina)
    {
        return true;
    }
    return false;
}

/**
 * @brief konvertuje zadate X i Y u polje sveta
 * 
 * @param x 
 * @param y 
 * @return int 
 */
int xyUSvet(int x, int y)
{
    int rezultat;
    rezultat = (y - 1) * velicinaSvetaX + (x - 1);
    return rezultat;
}

/**
 * @brief Upisivanje rezultata simulacije u fajl
 */
void upisiRezultateUFajl()
{
    ofstream fout;
    int i;

    fout.open("rezultat.txt", ios::out);
    if (!fout)
    {
        cout << "Greska u otvaranju datoteke.";
        exit(1);
    }

    fout << "Simulacija Lisica i zeceva uspesno zavrsena! \n\n";
    fout << "Parametri sa kojima ste zapoceli simulaciju su: \n";
    fout << "Lisice:" << brojLisica << "\n";
    fout << "Zecevi:" << brojZeceva << "\n";
    fout << "Zbunovi:" << brojZbunova << "\n\n";

    fout << "Broj zeceva koji su pojedeni tokom simulacije je: " << brojSmrti << "\n\n";
    fout << "Hvala!";

    fout.close();
}

int main()
{
    // Resetujem random seed
    srand(time(NULL));

    // Korisnik unosi parametre
    unosParametara();

    // Inicijalizjuemo zivotinje i svet
    lisice = new Lisica[brojLisica];
    zecevi = new Zec[brojZeceva];
    zbunovi = new Zbun[brojZbunova];
    svet = new int[velicinaSvetaX * velicinaSvetaY];

    inicijalizacijaObjekata();
    simulirajSvet();
    upisiRezultateUFajl();

    return 0;
}