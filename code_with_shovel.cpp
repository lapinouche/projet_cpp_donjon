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
#include <limits>

// to allow the game to "sleep" some seconds
//#include <chrono>
//#include <thread>

//Not use yet
/*
#include <memory>
#include <math.h>
#include <array>
#include <map>
#include <stdexcept> // for throwing error
*/

using namespace std;

enum class TypeCase {
    MUR,
    PASSAGE,
    TRESOR,
    MONSTRE,
    PIEGE, 
    PELLE,
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

class Pelle : public Case {
    private :
        int nbp;

    public :
        Pelle (int p = 0, const bool& visiter = false) : Case(visiter), nbp(p) {}
        TypeCase getType() const override { return TypeCase::PELLE; }
        char afficher () override { return '>'; }
        void SetNbp () { this->nbp++; }
        void UseNbp() { this->nbp--;}
        int GetNbp () const { return nbp; }
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

        void afficherStatut(Pelle& p){
            cout << "position courante : ";
            cout << "(" << position.first << ", " << position.second << ")" << endl;
            cout << "points de vie : " << sante << "/100" << endl;
            cout << "Inventaire : " << inventaire << " tresors" << endl;
            cout << "Pelle : " << p.GetNbp() << endl;
        }

        void setSante(int point_vie){ this->sante += point_vie; }
        int getSante() { return this->sante; }
        void SanteNull() { this->sante = 0; }
        void setInventaire(int newelement){ this->inventaire += newelement; }
        int getInventaire() { return this->inventaire; }
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
                case TypeCase::PELLE:{
                    return new Pelle();
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
        int nb_tresors;
        bool condEntree;

    public:
        Donjon() : largeur(0), hauteur(0), nb_tresors(0), condEntree(true) {}
        Donjon(const vector<vector<Case*>>& g, const int l=0, const int h=0, const int nt=0, const bool cE=true) : largeur(l), hauteur(h), grille(g), nb_tresors(nt), condEntree(cE) {}

        int getNT() { return this->nb_tresors;}
        void SetNT() { this->nb_tresors--;}

        void afficher(Aventurier& perso){
            cout << "\x1b[H" << flush;
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
            grille[1][1] = CaseFactory::creerCase(TypeCase::ENTREE); 
        }

        void poserSortie() {
            // Note: Use largeur-2/hauteur-2 if the edges are walls!
            delete grille[largeur-2][hauteur-2];
            grille[largeur-2][hauteur-2] = CaseFactory::creerCase(TypeCase::SORTIE);
        }

        
        void poserAventurier(int x, int y) {
            // Note: Use largeur-2/hauteur-2 if the edges are walls!
            delete grille[x][y];
            grille[x][y] = CaseFactory::creerCase(TypeCase::AVENTURIER);
        }

        void initialiserGrille(int largeur, int hauteur){ // void ou vector<vector<Case*>> !?
            cout << "nombre de tresors minimum requis pour finir le jeu : " << int(0.8*nb_tresors) << endl;
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
                            this->nb_tresors+=1;
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
                        else if (r < 16) {
                            delete grille[i][j];
                            grille[i][j] = CaseFactory::creerCase(TypeCase::PELLE);
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

        void resoudreCase(Aventurier& perso, Pelle& p, Case* c) {
            if (c->getType() == TypeCase::MONSTRE){
                cout << "\x1b[2J\x1b[H"; // full clear so the menu is clean
                string choix = "fuire";
                cout << "Voulez vous combatre ou fuire ? (c ou f)" << endl;
                cin >> choix;

                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear the buffer so the 'Enter' key doesn't ghost into the next move
                cout << "\x1b[2J\x1b[H";
                afficher(perso);

                if (choix == "c"){
                    int r = rand() % 100;
                    if (r < 50) { // valeur choisi arbitrairement 
                        cout << "Combat perdu" << endl;
                        if (perso.getSante() > 40){
                            perso.setSante(-60); // valeur choisi arbitrairement 
                        } else{
                            perso.SanteNull();
                        }
                    } else {
                        cout << "Combat gagné" << endl;
                        if (perso.getSante() < 81) {
                            perso.setSante(20); // valeur choisi arbitrairement 
                        }
                    }
                    system("pause");
                }
                else if (choix == "f") {
                    perso.updatePos(perso.getLastPos());
                }
                else {
                    cout << "saisie incorrect" << endl;
                    resoudreCase(perso, p, c);
                }
            }
            else if (c->getType() == TypeCase::TRESOR){
                perso.setInventaire(1);
                SetNT();
                pair<int, int> position = perso.getPos();
                delete grille[position.first][position.second];
                grille[position.first][position.second] =  CaseFactory::creerCase(TypeCase::PASSAGE);
            }
            else if (c->getType() == TypeCase::PIEGE){
                perso.setInventaire(-1); // valeur choisi arbitrairement
            }
            else if ((c->getType() == TypeCase::PELLE)){
                p.SetNbp();
                pair<int, int> position = perso.getPos();
                delete grille[position.first][position.second];
                grille[position.first][position.second] =  CaseFactory::creerCase(TypeCase::PASSAGE);
            }
        }

        void CreerChemin(int x, int y){ // pour appliquer l'effet de creusage avec la pelle
            delete grille[x][y];
            grille[x][y] = CaseFactory::creerCase(TypeCase::PASSAGE);
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

void execute_touche(Donjon& d, BFS& bfs, Aventurier& perso, Pelle& p, pair<int, int>& pos, int& nx, int& ny, int& nbtad) {
    cout << "\x1b[2J\x1b[H" << flush;
    if (nx == (d.getLargeur() - 2) && ny == (d.getHauteur() - 2) && (int(0.8*nbtad) -  perso.getInventaire()) > 0) {
        cout << "Il vous faut encore trouver au moins "; 
        cout << int(0.8*nbtad) - perso.getInventaire(); // update nb tresors : d.getNT()
        cout << " pour finir la partie !" << endl;
    } else if (d.casevalide(nx, ny)){
        stringstream frame;
        perso.updateLastPos(pos);
        d.deplacer(perso, nx, ny);
        d.resoudreCase(perso, p, d.getGrille(nx, ny));
        d.afficher(perso);
        perso.afficherStatut(p);
        bfs.AffichDist(d, nx, ny);
    } else if (p.GetNbp() > 0 && nx > 1 && nx <= d.getLargeur()-2 && ny > 1 && ny <= d.getHauteur()-2) {
        cout << "\x1b[2J\x1b[H"; // full clear so the menu is clean
        string choix = "v";
        cout << "Voulez vous creer un passage ? (v ou f)" << endl;
        cin >> choix;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\x1b[2J\x1b[H"; // Just added
        d.afficher(perso);
        if (choix == "v"){
            stringstream frame;
            cout << "\x1b[2J\x1b[H" << flush;
            d.CreerChemin(nx, ny);
            p.UseNbp();
            perso.updateLastPos(pos);
            d.deplacer(perso, nx, ny);
            d.resoudreCase(perso, p, d.getGrille(nx, ny));
            d.afficher(perso);
            perso.afficherStatut(p);
            bfs.AffichDist(d, nx, ny);
        }
    } else {
        cout << "Vous ne pouvez pas allez dans cette direction, veuillez en choisir une autre !" << endl;
    }
}

void boucleDeJeu(Donjon& d, Aventurier& perso, BFS& bfs, Pelle& p){
    int largeur = 21;
    int hauteur = 11;
    int nbtad = d.getNT(); // nombre de tresors disponible au debut du jeu
    
    // 1. Initialize ONCE
    d.initialiserGrille(largeur, hauteur);

    // 2. Clear screen once at start
    cout << "\x1b[2J\x1b[H"; // flush just added
    d.afficher(perso);
    perso.afficherStatut(p);

    //queue<pair<int, int>> chemin = bfs.trouverChemin(d, {1, 1}, {largeur-2, hauteur-2}); // {0, 0}, {21, 11}  --  {grille[0].size(), grille.size()}

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

            if (!perso.estVivant() || perso.getInventaire() + d.getNT() < int(0.8*nbtad)){ // Si  le personnage n'a plus de vie ou le nombre de tresors posséder par le personnage plus le nombre de tresors restant sur le plateau est inférieur au nombre de tresors requis pour terminer le jeu
                // 1. Clear the entire screen (\x1b[2J) 
                // 2. Move cursor to top-leff (\x1b[H)
                cout << "\x1b[2J\x1b[H"; // Remetre le cursor en haut
                if (perso.estVivant()){
                    cout << "========================" << endl;
                    cout << "    Niveau perdu :(     " << endl;
                    cout << "========================" << endl;
                } else{
                    cout << "===========================================================" << endl;
                    cout << " Vous avez perdu trop de tresors pour pouvoir finir le jeu " << endl;
                    cout << "                       Niveau perdu :(                     " << endl;
                    cout << "===========================================================" << endl;
                }
                cout << "Appuyer sur une touche pour quitter..." << endl;
                cin.ignore(); // Nettoyer toute precedente ligne dans le terminal (buffer)
                cin.get();    // Attendre pour quitter le jeu
                running = false;
            } else if (perso.getPos() == make_pair(largeur-2, hauteur-2)){
                cout << "\x1b[2J\x1b[H";
                cout << "===========================" << endl;
                cout << "   Niveau termine !!! :)   " << endl;
                cout << "===========================" << endl;
                cout << "Appuyer sur une touche pour quitter..." << endl;
                cin.ignore(); // Nettoyer toute precedente ligne dans le terminal (buffer)
                cin.get(); // Attendre pour quitter le jeu
                running = false;
            } else if (nx != pos.first || ny != pos.second){
                execute_touche(d, bfs, perso, p, pos, nx, ny, nbtad);
            }
        }
    }
}

int main(){
    srand(time(NULL));
    Donjon d;
    BFS bfs;
    Aventurier perso;
    Pelle p;
    cout << "\x1b[?1049h\x1b[?25l" << flush;
    boucleDeJeu(d, perso, bfs, p);
    cout << "\x1b[?25h\x1b[?1049l" << flush;
    //cout << "\x1b[?1049l\x1b[?25h" << flush;
    return 0;
}
