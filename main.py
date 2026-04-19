#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <cstdlib> // for random
#include <math.h>
#include <queue>
#include <map>
//#include "Case.cpp"

using namespace std;

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
        if (type == TypeCase::MUR) {
            return new Mur();
        } else if (type == TypeCase::PASSAGE) {
            return new Passage();
        } else if (type == TypeCase::TRESOR) {
            return new Tresor();
        } else if (type == TypeCase::MONSTRE) {
            return new Monstre();
        } else if (type == TypeCase::PIEGE) {
            return new Piege();
        } else {
            //throw std::invalid_argument("Type de case inconnu");
            return nullptr;
        }
    }
};

class Case{
    protected:
        bool visiter;

    public:
        Case(const bool& visiter = false) : visiter(visiter) {}

        virtual char afficher() = 0;
        virtual ~Case() = default;

        virtual bool SetV(const bool newstate){
            visiter = newstate;
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
                    grille[i][j] = CaseFactory::creerCase(TypeCase::MUR);
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

        vector<pair<int, int>> trouverChemin(){

        }

        vector<vector<Case*>> genererLabyrinthe(vector<vector<Case*>> grille, int x, int y){
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
                    Passage* p;
                    Case* grille[nx][ny]{p};
                    return genererLabyrinthe(this->grille, nx, ny);
                }
                return grille;
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
                    grille[i][j] = CaseFactory::creerCase(TypeCase::MUR);
                }
            }
            genererLabyrinthe(grille, 1, 1);
            poserEntree(grille);
            poserSortie(grille);
        }

        void placerElement(vector<vector<Case*>>& grille){
            for (int i=0; i < this->largeur; i++){
                for (int j=0; j < this->hauteur; j++){
                    if (typeid(grille[i][j][0]) == typeid(Passage)){
                        int r = rand() % 100;
                        if (r < 5){
                            grille[i][j] = CaseFactory::creerCase(TypeCase::TRESOR);
                        }
                        else if (r < 10) {
                            grille[i][j] = CaseFactory::creerCase(TypeCase::MONSTRE);
                        }
                        else if (r < 13) {
                            grille[i][j] = CaseFactory::creerCase(TypeCase::PIEGE);
                        }
                    }
                }
            }
        }
};


class BFS{
    private:
        queue<int> file;
        vector<bool> visite;
        vector<int> parent;

    public:
        BFS(const queue<int> f = {}, const vector<bool> v = {}, const vector<int> p = {}) : file(f), visite(v), parent(p) {}
        
        queue<int> trouverChemin(vector<vector<Case*>> grille, int depart, int arrivee){
            queue<int> file;
            vector<bool> visite;
            vector<int> parent;
        
            enfiler(file, depart);
            visite[depart] = true;

            this->visite[depart] = true;
            while(file.empty() == false){
                int courant = file.front(); // or back !??#?
                if (courant == arrivee){
                    return reconstruireChemin(parent, depart, arrivee);
                }
            }
        }

        queue<int> reconstruireChemin(const vector<int>& parent, int depart, int arrivee){
            queue<int> chemin;
            int courant = arrivee;

            while (courant != depart){
                chemin.push(courant);
                courant = parent[courant];
            }

            chemin.front() = depart;
            return chemin;
        }

        void enfiler(queue<int>& file, int depart){
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

    /*
    BFS bfs();
    bfs.trouverChemin(grille, 0, 10);
    bfs.reconstruireChemin();
    */

    return 0;
}
