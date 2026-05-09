#include <iostream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <iostream>
using namespace std;

#include "donjon.h"
#include "aventurier.h"


int main() {
    srand(time(NULL));
    Aventurier perso(1, 1);
    Donjon d;
    d.generer(15, 15); // Généré UNE SEULE FOIS
    perso.afficherMenu(d);
    return 0;
}
