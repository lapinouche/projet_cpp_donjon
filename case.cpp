#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <cstdlib> // for random
#include <math.h>
#include <queue>

using namespace std;
// Classe abstraite de base

class CaseFactory{
    public:
        CaseFactory(){}

        void creerCase(const Case& TypeCase){
            TypeCase NewCase();
        }
};

class Case{
    private:
        bool visiter;

    public:
        Case(const bool& visiter=false) : visiter(visiter) {}

        virtual char afficher() = 0;
        virtual ~Case() = default;
};

class Mur : public Case {
public : 

    char afficher () override {
        return '#' ;
    }

};


class Passage : public Case {
public : 

    char afficher () override {
        return ' ' ;
    }

};

class Tresor : public Case {
private :

    int valeur ;

public :

    Tresor ( int v = 10 ) : valeur ( v ) {}
    char afficher () override {
        return '+' ;
    }

    int getValeur () const {
        return valeur ;
    }

};

class Monstre : public Case {
private :

    int pv ;

public :

    Monstre ( int p = 20 ) : pv ( p ) {}
    char afficher () override {
        return 'M' ;
    }

    int getPv () const {
        return pv ;
    }

};

class Piege : public Case {
private :

    int degats ;

public :

    Piege ( int d = 15 ) : degats ( d ) {}
    char afficher () override {
        return 'T' ;
    }

    int getDegats () const {
        return degats ;
    }

};

class Donjon {
    private:
        vector<vector<Case*>> grille;

    public:
        Donjon(const vector<vector<Case*>>& grille) : grille(grille) {}

        void generer(int largeur, int hauteur){

        }

        void afficher(){
            for (int i=0; i < sqrt(grille.size()); i++){
                for (int j=0; j < sqrt(grille.size()); j++){
                    Case* g = grille[i][j];
                    cout << g->afficher();
                }
                cout << endl;
            }
            cout << endl;
        }

        vector<pair<int, int>> trouverChemin(){

        }

        void genererLabyrinthe(vector<vector<Case*>> grille, int x, int y){
            grille[x][y].visiter &= true;
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

                bool c1 = (0 < nx < sqrt(grille.size()));
                bool c2 = (0 < ny < sqrt(grille.size()));

                if (c1 == true && c2 == true){
                    Passage* p;
                    Case* grille[nx][ny] = p;
                    genererLabyrinthe(this->grille, nx, ny);
                }
            }
        }

        Case* poserEntree(const vector<vector<Case*>>& grille){
            Case* entree = grille[0][0];
            return entree;
        }

        Case* poserSortie(const vector<vector<Case*>>& grille){
            int s = sqrt(grille.size());
            Case* sortie = grille[s][s];
            return sortie;
        }

        void initialiserGrille(int largeur, int hauteur){
            for (int i=0; i<largeur; i++){
                for (int j=0; j<hauteur; j++){
                    *grille[i][j] = *CaseFactory::creerCase(TypeCase::MUR);
                }
            }
            genererLabyrinthe(grille, 1, 1);
            poserEntree(grille);
            poserSortie(grille);
        }

        void placerElement(const vector<vector<Case*>>& grille){
            for (int i=0; i<20; i++){
                for (int j=0; j<20; j++){
                    if (typeid(grille[i][j])) == Passage){
                        int r = rand() % 100;
                        if (r < 5){
                            *grille[i][j] = *CaseFactory::creerCase(TypeCase::TRESOR);
                        }
                        else if (r < 10) {
                            *grille[i][j] = *CaseFactory::creerCase(TypeCase::MONSTRE);
                        }
                        else if (r < 13) {
                            *grille[i][j] = *CaseFactory::creerCase(TypeCase::PIEGE);
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
        void trouverChemin(vector<vector<Case*>> grille, int depart, int arrivee){
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

        void reconstruireChemin(const vector<int>& parent, int depart, int arrivee){
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
    return 0;
}
