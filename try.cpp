#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // for random
#include <queue>
#include <utility> // for pair
#include <typeinfo>

#include <cstdlib> // for redraw the map an UI every frame
#include <conio.h>
#include <sstream>

#include <ctime>
#include <algorithm>

using namespace std;

enum class TypeCase {
    MUR,
    PASSAGE,
    TRESOR,
    MONSTRE,
    PIEGE, 
    ENTREE,
    SORTIE,
    AVENTURIER
};

class Case{
    public:
        bool visiter;
        Case(const bool& visiter = false) : visiter(visiter) {}
        virtual ~Case() = default;

        virtual char afficher() = 0;
        virtual TypeCase getType() const = 0; // Must be const to match children
        
        void SetV(bool newstate){ this->visiter = newstate; }
        bool getV() const{ return visiter; }
};

class Mur : public Case {
    public :
        Mur (const bool& visiter = false) : Case(visiter) {}
        TypeCase getType() const override { return TypeCase::MUR; }
        char afficher () override { return '#' ; }
};


class Passage : public Case {
    public : 
        Passage(const bool& visiter = false) : Case(visiter) {}
        TypeCase getType() const override { return TypeCase::PASSAGE; }
        char afficher () override { return ' ' ; }
};

class Tresor : public Case {
    private :
        int valeur ;
    public :
        Tresor (int v = 10, const bool& visiter = false) : Case(visiter), valeur(v) {}
        TypeCase getType() const override { return TypeCase::TRESOR; }
        char afficher () override { return '+'; }
        void setValeur (int valeur) { this->valeur = valeur; }
        int getValeur () const { return valeur ; }
};

class Monstre : public Case {
    private :
        int pv ;
    public :
        Monstre (int p = 20, const bool& visiter = false) :  Case(visiter), pv(p) {}
        TypeCase getType() const override { return TypeCase::MONSTRE; }
        char afficher () override { return 'M'; }
        void setPv (int pv) { this->pv = pv; }
        int getPv () const { return pv ; }
};

class Piege : public Case {
    private :
        int degats ;

    public :
        Piege (int d = 15, const bool& visiter = false) : Case(visiter), degats(d) {}
        TypeCase getType() const override { return TypeCase::PIEGE; }
        char afficher () override { return 'T'; }
        void setDegats (int degats) { this->degats = degats; }
        int getDegats () const { return degats; }
};

class Aventurier : public Case {
    private:
        pair<int, int> last_pos; // à mettre à jour lors avant le deplacement du joueur dans la boucle de jeu
        pair<int, int> position;
        int sante; // point de vie du joueur (initialiser à 100/100)
        int inventaire; // nombre de tresor ramasser par le joueur

    public:
        Aventurier() : last_pos({1, 1}), position({1, 1}), sante(100), inventaire(0) {}
        Aventurier(const pair<int, int>& last_pos, const pair<int, int>& position, const int sante, const int inventaire) : last_pos(last_pos), position(position), sante(sante), inventaire(inventaire) {}
        TypeCase getType() const override { return TypeCase::AVENTURIER; }
        char afficher () override { return '@'; }

        bool estVivant(){
            if (sante > 0){
                return true;
            }
            return false;
        }

        void afficherStatut(){
            cout << "position courante : ";
            cout << "(" << position.first << ", " << position.second << ")" << endl;
            cout << "points de vie : " << sante << "/100" << endl;
            cout << "Inventaire : " << inventaire << " tresors" << endl;
        }

        void setSante(int point_vie){ this->sante += point_vie; }
        int getSante() { return this->sante; }
        void SanteNull() { this->sante = 0; }
        void setInventaire(int newelement){ this->inventaire += newelement; }
        void updatePos(pair<int, int> newPos){ this->position = newPos; }
        void updateLastPos(pair<int, int> newPos){ this->last_pos = newPos; }
        pair<int, int> getPos() { return this->position; }
        int getXP() { return this->position.first; }
        int getYP() { return this->position.second; }
        pair<int, int> getLastPos() { return this->last_pos; }
        int getXLP() { return this->last_pos.first; }
        int getYLP() { return this->last_pos.second; }
};

class Entree : public Case {
    public:
        TypeCase getType() const override { return TypeCase::ENTREE; }
        char afficher() override { return 'E'; }
};

class Sortie : public Case {
    public:
        TypeCase getType() const override { return TypeCase::SORTIE; }
        char afficher() override { return 'S'; }
};

class CaseFactory {
    public:
        static Case* creerCase(TypeCase type) {
            switch(type){
                case TypeCase::MUR:{
                    return new Mur();
                }
                case TypeCase::PASSAGE:{
                    return new Passage();
                }
                case TypeCase::TRESOR:{
                    return new Tresor();
                }
                case TypeCase::MONSTRE:{
                    return new Monstre();
                }
                case TypeCase::PIEGE:{
                    return new Piege();
                }
                case TypeCase::ENTREE:{
                    return new Entree();
                }
                case TypeCase::SORTIE:{
                    return new Sortie();
                }
                case TypeCase::AVENTURIER:{
                    return new Aventurier();
                }
                default:{
                    return nullptr;
                }
            }
        }
};

class Donjon {
    protected:
        vector<vector<Case*>> grille;
        int largeur;
        int hauteur;
        bool condEntree;

    public:
        Donjon() : largeur(0), hauteur(0), condEntree(true) {}
        Donjon(const vector<vector<Case*>>& g, const int l=0, const int h=0, const bool cE=true) : largeur(l), hauteur(h), grille(g), condEntree(cE) {}

        void afficher(Aventurier& perso){
            for (int j=0; j < hauteur; j++){
                for (int i=0; i < largeur; i++){
                    if (i == perso.getXP() && j == perso.getYP()) {
                        cout << '@';
                    } else {
                        cout << grille[i][j]->afficher();
                    }
                }
                cout << endl;
            }
        }

        Case* getGrille(int i, int j) { return this->grille[i][j]; }
        int getLargeur() { return this->largeur; }
        int getHauteur() { return this->hauteur; }

        vector<string> melanger(vector<string> directions){ // à modifier
            for (int i = directions.size() - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                swap(directions[i], directions[j]);
            }
            return directions;
        }

        void generer(int largeur, int hauteur){
            this->largeur = largeur;
            this->hauteur = hauteur;
            grille.assign(largeur, vector<Case*>(hauteur, nullptr));
            
            for (int i=0; i < largeur; i++){
                for (int j=0; j < hauteur; j++){
                    grille[i][j] = CaseFactory::creerCase(TypeCase::MUR);
                }
            }
        }

        void genererLabyrinthe(int x, int y){ 
            grille[x][y]->SetV(true); // error : Assertion '__n < this->size()' failed.
            vector<string> directions = {"NORD", "SUD", "EST", "OUEST"};
            directions = melanger(directions);
            /*
            cout << "direction : ";
            for(string d : directions){
                cout << d << " ";
            } 
            cout << endl;*/

            for (string d : directions){
                // nouvelle case
                int nx, ny;
                // case entre la position actuelle et la nouvelle case
                int mx, my;

                if (d == "NORD"){
                    nx = x; ny = y + 2;
                    mx = x; my = y + 1;
                }
                if (d == "SUD"){
                    nx = x; ny = y - 2; 
                    mx = x; my = y - 1; 
                }
                if (d == "EST"){
                    nx = x + 2; ny = y;
                    mx = x + 1; my = y;
                }
                if (d == "OUEST"){
                    nx = x - 2; ny = y; 
                    mx = x - 1; my = y;
                }

                bool boundsCheck = (nx >= 0 && nx < largeur && ny >= 0 && ny < hauteur);
                
                if (boundsCheck){ 
                    if (!grille[nx][ny]->getV()){
                        // Carve a path
                        delete grille[mx][my];
                        grille[mx][my] = CaseFactory::creerCase(TypeCase::PASSAGE);
                        delete grille[nx][ny];
                        grille[nx][ny] = CaseFactory::creerCase(TypeCase::PASSAGE);
                        
                        genererLabyrinthe(nx, ny);
                    }
                }
            }
        }

        void poserEntree() {
            delete grille[1][1];
            grille[1][1] = new Entree(); 
        }

        void poserSortie() {
            // Note: Use largeur-2/hauteur-2 if the edges are walls!
            delete grille[largeur-2][hauteur-2];
            grille[largeur-2][hauteur-2] = new Sortie();
        }

        
        void poserAventurier(int x, int y) {
            // Note: Use largeur-2/hauteur-2 if the edges are walls!
            delete grille[x][y];
            grille[x][y] = new Aventurier();
        }

        void initialiserGrille(int largeur, int hauteur){ // void ou vector<vector<Case*>> !?
            generer(largeur, hauteur);
            genererLabyrinthe(1, 1);
            poserSortie();
            poserAventurier(1, 1);
            placerElement();
        }

        vector<pair<int, int>> trouverChemin(int x, int y){
            vector<string> directions = {"NORD", "SUD", "EST", "OUEST"};
            for (string d : directions){
                int nx;
                int ny;

                if (d == "NORD"){
                    nx = x;
                    ny = y + 1;
                }
                if (d == "SUD"){
                    nx = x;
                    ny = y - 1;
                }
                if (d == "EST"){
                    nx = x + 1;
                    ny = y;
                }
                if (d == "OUEST"){
                    nx = x - 1;
                    ny = y;
                }
                bool c1 = (nx >= 0 && nx < largeur);
                bool c2 = (ny >= 0 && ny < hauteur);

                if (c1 == true && c2 == true){
                    if (grille[nx][ny]->getType() == TypeCase::PASSAGE){
                        vector<pair<int, int>> chemin = {{nx, ny}};
                        return chemin;
                    }
                }
            }
            cout << "Aucun chemin trouve !" << endl;
            return {};
        }

        void placerElement(){ // vector<vector<Case*>>& grille
            for (int j=0; j < hauteur; j++){
                for (int i=0; i < largeur; i++){
                    if (grille[i][j]->getType() == TypeCase::PASSAGE){
                        int r = rand() % 100;
                        if (r < 5) {
                            delete grille[i][j];
                            grille[i][j] = CaseFactory::creerCase(TypeCase::TRESOR);
                        } 
                        else if (r < 10) {
                            delete grille[i][j];
                            grille[i][j] = CaseFactory::creerCase(TypeCase::MONSTRE);
                        }
                        else if (r < 13) {
                            delete grille[i][j];
                            grille[i][j] = CaseFactory::creerCase(TypeCase::PIEGE);
                        } 
                    }
                }
            }
        }

        bool casevalide(int x, int y){
            if (x < 0 || x >= largeur || y < 0 || y >= hauteur ){
                return false;
            }
            return grille[x][y]->getType() != TypeCase::MUR;
        }

        void deplacer(Aventurier& perso, int nx, int ny){
            // precondition : la case (nx, ny) est franchissable (pas un mur)
            if (casevalide(nx, ny) == true){
                perso.updatePos({nx, ny});
                if (nx == 1 || ny == 1){
                    if ((!(nx == 1 && ny == 1)) && condEntree){
                        poserEntree();
                        condEntree = false;
                    }
                }
            }
        }

        void resoudreCase(Aventurier& perso, Case* c) {
                if (c->getType() == TypeCase::MONSTRE){
                        cout << "\x1b[2J\x1b[H"; // full clear so the menu is clean
                        string choix = "fuire";
                        cout << "Voulez vous combatre ou fuire ?" << endl;
                        cin >> choix;

                        cin.ignore(1000, '\n'); // Clear the buffer so the 'Enter' key doesn't ghost into the next move

                        if (choix == "combatre"){
                                int r = rand() % 100;
                                if (r < 50) { // valeur choisi arbitrairement 
                                        cout << "Combat perdu" << endl;
                                        if (perso.getSante() > 40){
                                            perso.setSante(-40); // valeur choisi arbitrairement 
                                        }
                                        else{
                                            perso.SanteNull();
                                        }
                                }
                                else {
                                        cout << "Combat gagné" << endl;
                                        if (perso.getSante() < 81) {
                                            perso.setSante(20); // valeur choisi arbitrairement 
                                        }
                                }
                                system("pause");
                        }
                        else if (choix == "fuire") {
                                perso.updatePos(perso.getLastPos());
                        }
                        else {
                                cout << "saisie incorrect" << endl;
                                resoudreCase(perso, c);
                        }
                        cout << "\x1b[2J\x1b[H";
                }
                else if (c->getType() == TypeCase::TRESOR){
                        perso.setInventaire(1);
                        pair<int, int> position = perso.getPos();
                        delete grille[position.first][position.second];
                        grille[position.first][position.second] =  CaseFactory::creerCase(TypeCase::PASSAGE);
                }
                else if (c->getType() == TypeCase::PIEGE){
                        perso.setInventaire(-1); // valeur choisi arbitrairement
                }
        }
};

class BFS {
    private:
        queue<pair<int, int>> file;
        vector<vector<pair<int, int>>> parent;

    public:
        BFS() = default; // tells the compiler to use the default empty state for both containers
        BFS(const queue<pair<int, int>> f, const vector<vector<pair<int, int>>>& p) : file(f), parent(p) {}

        void AffichDist(Donjon& d, int x, int y){
            int largeur = d.getLargeur();
            int hauteur = d.getHauteur();
            queue<pair<int, int>> chemin = trouverChemin(d, {x, y}, {largeur-2, hauteur-2});
            int dist = chemin.size();
            cout << "Distance a la sortie : " << dist << " cases" << endl;
        }
        
        queue<pair<int, int>> trouverChemin(Donjon& d, pair<int, int> depart = {1, 1}, pair<int, int> arrivee = {5, 0}){
            int largeur = d.getLargeur();
            int hauteur = d.getHauteur();
            
            if (largeur == 0 || hauteur == 0) return {}; // Safety check

            queue<pair<int, int>> file;
            vector<vector<bool>> visite(largeur, vector<bool>(hauteur, false));
            vector<vector<pair<int, int>>> parent(largeur, vector<pair<int, int>>(hauteur, {-1, -1}));
        
            file.push(depart); //enfiler(file, depart)
            visite[depart.first][depart.second] = true;
            
            while(!file.empty()){
                pair<int, int> courant = file.front();
                file.pop();

                if (courant == arrivee) return reconstruireChemin(parent, depart, arrivee);

                // Check 4 directions (N, S, E, O) 
                // Note: Using i-1 to i+1 in a nested loop checks diagonals too. 
                // If your game only allows 4-way movement, use a directions vector.
                int dx[] = {0, 0, 1, -1};
                int dy[] = {1, -1, 0, 0};

                for (int i = 0; i < 4; i++) {
                    int nx = courant.first + dx[i];
                    int ny = courant.second + dy[i];

                    // PROPER BOUNDS CHECK
                    if (nx >= 0 && nx < largeur && ny >= 0 && ny < hauteur) {
                        if (d.getGrille(nx, ny)->getType() != TypeCase::MUR && !visite[nx][ny]) {
                            visite[nx][ny] = true;
                            parent[nx][ny] = courant;
                            file.push({nx,ny}); //enfiler(file, v)
                        }
                    }
                }
            }
            return {};
        }

        queue<pair<int, int>> reconstruireChemin(const vector<vector<pair<int, int>>>& parent, pair<int, int> depart, pair<int, int> arrivee){
            queue<pair<int, int>> chemin;
            pair<int, int> courant = arrivee;

            while (courant != depart){
                chemin.push(courant);
                courant = parent[courant.first][courant.second];
            }
            if (!chemin.empty()){
                chemin.front() = depart;
            }
            return chemin;
        }
};

void execute_touche(Donjon& d, BFS& bfs, Aventurier& perso, pair<int, int>& pos, int& nx, int& ny) {
    if (d.casevalide(nx, ny)){
        stringstream frame;
        perso.updateLastPos(pos);
        d.deplacer(perso, nx, ny);
        d.resoudreCase(perso, d.getGrille(nx, ny));
        d.afficher(perso);
        perso.afficherStatut();
        bfs.AffichDist(d, nx, ny);
    } else {
        cout << "Vous ne pouvez pas allez dans cette direction, veuillez en choisir une autre !" << endl;
    }
}

void boucleDeJeu(Donjon& d, Aventurier& perso, BFS& bfs){
    int largeur = 21;
    int hauteur = 11;
    
    // 1. Initialize ONCE
    d.initialiserGrille(largeur, hauteur);

    // 2. Clear screen once at start
    cout << "\x1b[2J\x1b[H";
    d.afficher(perso);
    perso.afficherStatut();

    bool running = true;
    while (running){
        if (_kbhit()) { // Check if a key was pressed
            char key = _getch(); // Get the character without displaying it
            pair<int, int> pos = perso.getPos();
            int nx = pos.first;
            int ny = pos.second;

            switch (key) {
                case 'z': case 'Z':
                    ny--;
                    break;
                case 's': case 'S':
                    ny++;
                    break;
                case 'q': case 'Q':
                    nx--;
                    break;
                case 'd': case 'D':
                    nx++;
                    break;
                    
            }

            if (nx != pos.first || ny != pos.second){
                cout << "\x1b[H";
                execute_touche(d, bfs, perso, pos, nx, ny);
            }
            
            if (!perso.estVivant() || perso.getPos() == make_pair(largeur-2, hauteur-2)){
                running = false;
                if (!perso.estVivant()) cout << "Niveau Perdu :(" << endl;
                else cout << "Niveau termine !!! :)" << endl;
            }
        }
    }
}

int main(){
    srand(time(NULL));
    Donjon d;
    BFS bfs;
    Aventurier perso;
    boucleDeJeu(d, perso, bfs);
    return 0;
}

// https://www.pointerlab.fr/blog/cpp-std-queue
// https://cplusplus.com/forum/general/94461/
