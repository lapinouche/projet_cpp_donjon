#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <cstdlib> // for random
#include <math.h>
#include <queue>
#include <map>
#include <memory>

using namespace std;

class Case{
    protected:
        bool visiter;

    public:
        Case(const bool& visiter = false) : visiter(visiter) {}

        virtual char afficher() = 0;
        virtual ~Case() = default;

        bool SetV(const bool newstate){
            visiter = newstate;
            return visiter;
        }
};

class Mur : public Case {
    public :
        Mur (const bool& visiter = false) : Case(visiter) {}
        char afficher () override {
            return '#' ;
        }
};


class Passage : public Case {
    public : 
        Passage(const bool& visiter = false) : Case(visiter) {}
        char afficher () override {
            return ' ' ;
        }
};

class Tresor : public Case {
    private :
        int valeur ;

    public :
        Tresor (int v = 10, const bool& visiter = false) : valeur(v), Case(visiter) {}
        char afficher () override {
            return '+';
        }

        int getValeur () const {
            return valeur ;
        }
};

class Monstre : public Case {
    private :
        int pv ;

    public :
        Monstre (int p = 20, const bool& visiter = false) : pv(p), Case(visiter) {}
        char afficher () override {
            return 'M';
        }

        int getPv () const {
            return pv ;
        }
};

class Piege : public Case {
    private :
        int degats ;

    public :
        Piege (int d = 15, const bool& visiter = false) : degats(d), Case(visiter) {}
        char afficher () override {
            return 'T';
        }

        int getDegats () const {
            return degats;
        }
};

class Aventurier{
    private:
        pair<int, int> position;
        int sante; // point de vie du joueur (initialiser à 100/100)
        int inventaire; // nombre de tresor ramasser par le joueur
    public:
        Aventurier(const pair<int, int>& position = {0, 0}, const int sante = 100, const int inventaire = 0) : position(position), sante(sante), inventaire(inventaire) {}

        void deplacer(int nx, int ny){
            // precondition : la case (nx, ny) est franchissable (pas un mur)
            if (Donjon::casevalide(nx, ny) == true){
                position = {nx, ny};
            }
        }

        bool estVivant(){
            if (this->sante > 0){
                return true;
            }
            return false;
        }

        void afficherStatut(){
            cout << "position courante : ";
            cout << "(" << position.first << ", " << position.second << ")" << endl;
            cout << "points de vie : " << sante << endl;
            cout << "contenu de l'inventaire : " << inventaire << endl;
        }
};

enum class TypeCase {
    MUR,
    PASSAGE,
    TRESOR,
    MONSTRE,
    PIEGE
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
                case TypeCase::PIEGE:{
                    return new Piege();
                }
                default:{
                    return nullptr;
                }
            }
        }
};

class Donjon{
    private:
        vector<vector<Case*>> grille;
        int largeur;
        int hauteur;

    public:
        Donjon(const vector<vector<Case*>>& grille = {}, const int l=0, const int h=0) : largeur(l), hauteur(h), grille(grille) {}

        void generer(int largeur, int hauteur){
            this->largeur = largeur;
            this->hauteur = hauteur;
            //map<int, Case*> piece = {{0, Mur()}, {1, Passage()}, {2, Tresor()}, {3, Monstre()}, {4, Piege()}};
            for (int i=0; i < largeur; i++){
                for (int j=0; j < hauteur; j++){
                    this->grille[i][j] = CaseFactory::creerCase(TypeCase::MUR);
                    //Mur* m;
                    //Case* grille[i][j]{m};
                }
            }
        }

        void afficher(){
            for (int i=0; i < this->largeur; i++){
                for (int j=0; j < this->hauteur; j++){
                    Case* g = &grille[i][j][0];
                    cout << g->afficher();
                }
                cout << endl;
            }
            cout << endl;
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

                bool c1 = (0 < nx < this->largeur);
                bool c2 = (0 < ny < this->hauteur);

                if (c1 == true && c2 == true){
                    if (typeid(grille[nx][ny][0]) == typeid(Passage)){
                        vector<pair<int, int>> chemin = {{nx, ny}};
                        return chemin;
                    }
                }
            }
            return {};
        }

        void genererLabyrinthe(vector<vector<Case*>> grille, int x, int y){ 
            grille[x][y][0].SetV(true);
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

                bool c1 = (0 < nx < this->largeur);
                bool c2 = (0 < ny < this->hauteur);

                if (c1 == true && c2 == true){
                    //Passage* p;
                    //Case* grille[nx][ny]{p};
                    grille[nx][ny] = CaseFactory::creerCase(TypeCase::PASSAGE);
                    return genererLabyrinthe(this->grille, nx, ny);
                }
            }
        }

        Case* poserEntree(const vector<vector<Case*>>& grille){
            Case* entree = grille[0][0];
            return entree;
        }

        Case* poserSortie(const vector<vector<Case*>>& grille){
            Case* sortie = grille[this->largeur][this->hauteur];
            return sortie;
        }

        vector<vector<Case*>> initialiserGrille(int largeur, int hauteur){
            for (int i=0; i < largeur; i++){
                for (int j=0; j < hauteur; j++){
                    this->grille[i][j] = CaseFactory::creerCase(TypeCase::MUR);
                    //Mur* m;
                    //Case* grille[i][j]{m};
                }
            }
            genererLabyrinthe(grille, 1, 1);
            poserEntree(grille);
            poserSortie(grille);
            return grille;
        }

        void placerElement(vector<vector<Case*>>& grille){
            for (int i=0; i < this->largeur; i++){
                for (int j=0; j < this->hauteur; j++){
                    if (typeid(grille[i][j][0]) == typeid(Passage)){
                        int r = rand() % 100;
                        if (r < 5){
                            grille[i][j] = CaseFactory::creerCase(TypeCase::TRESOR);
                            //grille[i][j].assign(1, CaseFactory::creerCase(TypeCase::TRESOR));
                            //Case* newtresor = CaseFactory::creerCase(TypeCase::TRESOR);
                            //grille[i][j] = newtresor;
                            //Tresor* m;
                            //Case* grille[i][j]{m};
                        }
                        else if (r < 10) {
                            grille[i][j] = CaseFactory::creerCase(TypeCase::MONSTRE);
                            //Case* newmonstre = CaseFactory::creerCase(TypeCase::MONSTRE);
                            //grille[i][j] = newmonstre;
                            //Monstre* m;
                            //Case* grille[i][j]{m};
                        }
                        else if (r < 13) {
                            grille[i][j] = CaseFactory::creerCase(TypeCase::PIEGE);
                            //Piege* p;
                            //Case* grille[i][j]{p};
                        }
                    }
                }
            }
        }

        static bool casevalide(int x, int y){
            if (typeid(grille[x][y]) != typeid(Mur)){
                return true;
            }
            return false;
        }
};

class BFS{
    private:
        queue<pair<int, int>> file;
        vector<vector<pair<int, int>>> parent;

    public:
        BFS(const queue<pair<int, int>> f = {}, const vector<vector<pair<int, int>>>& p = {}) : file(f), parent(p) {}
        
        queue<pair<int, int>> trouverChemin(vector<vector<Case*>> grille, pair<int, int> depart = {0, 0}, pair<int, int> arrivee = {5, 0}){
            queue<pair<int, int>> file = {};
            vector<vector<bool>> visite(grille.size(), vector<bool>(grille[0].size(),false));
            vector<vector<pair<int, int>>> parent(grille.size(), vector<pair<int, int>>(grille[0].size(), {-1, 1}));
        
            enfiler(file, depart);
            visite[depart.first][depart.second] = true;

            while(file.empty() == false){
                pair<int, int> courant = file.back(); // front or back !??#?
                if (courant == arrivee){
                    return reconstruireChemin(parent, depart, arrivee);
                }

                for (int i = (courant.first - 1) ; i < (courant.first + 2); i++){
                    for (int j = (courant.second - 1) ; j < (courant.second + 2); j++){
                        pair<int, int> v = {i, j};
                        if ((-1 < v.first < grille[0].size()) && (-1 < v.second < grille.size()) && visite[v.first][v.second] == false && typeid(grille[v.first][v.second]) != typeid(Mur)){
                            visite[v.first][v.second] = true;
                            parent[v.first][v.second] = courant;
                            enfiler(file, v);
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

            chemin.front() = depart;
            return chemin;
        }

        void enfiler(queue<pair<int, int>>& file, pair<int, int> depart){
            file.front() = depart;
        }
};

// https://www.pointerlab.fr/blog/cpp-std-queue

int main(){
    Donjon d;
    d.generer(22, 10);
    vector<vector<Case*>> grille = d.initialiserGrille(22, 10);
    d.placerElement(grille);
    d.afficher();

    BFS bfs;
    bfs.trouverChemin(grille, 0, 10);

    return 0;
}
