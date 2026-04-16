#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <cstdlib> // for random

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

        void generer(int largeur, int hauteur);

        void afficher();

        vector<pair<int, int>> trouverChemin();

        void genererLabyrinthe(vector<vector<Case*>> grille, int x, int y){
            grille[x][y].visiter = true;
            vector<string> directions = {"NORD", "SUD", "EST", "OUEST"};

            for (string d : directions){
                if (d == "NORD"){
                    int nx = x;
                    int ny = y + 1;
                }
                if (d == "SUD"){
                    int nx = x;
                    int ny = y - 1;
                }
                if (d == "EST"){
                    int nx = x + 1;
                    int ny = y;
                }
                if (d == "OUEST"){
                    int nx = x - 1;
                    int ny = y;
                }
                (l, h) = grille.size()
                if (){
                    // casser le mur entre (x, y) et (nx, ny)
                    genererLabyrinthe(grille, nx, ny);
                }


            }

        }

        Case poserEntree(const vector<vector<Case*>>& grille){
            Case entree = grille[0][0];
            return entree;
        }

        void poserSortie(const vector<vector<Case*>>& grille){
            Case sortie = grille[20][20];
            return sortie;
        }

        void initialiserGrille(int largeur, int hauteur){
            for (int i=0; i<largeur; i++){
                for (int j=0; j<hauteur; j++){
                    grille[i][j] = CaseFactory::creeCase(TypeCase::MUR)
                }
            }
            genererLabyrinthe(grille, 1, 1);
            poserEntree(grille);
            poserSortie(grille);
        }

        void placerElement(const vector<vector<Case*>>& grille){
            for (int i=0; i<20; i++){
                for (int j=0; j<20; j++){
                    if (grille[i][j] == PASSAGE){
                        int r = rand() % 100;
                        if (r < 5){
                            grille[i][j] = CaseFactory::creerCase(TRESOR);
                        }
                        else if (r < 10) {
                            grille[i][j] = CaseFactory::creerCase(MONSTRE);
                        }
                        else if (r < 13) {
                            grille[i][j] = CaseFactory::creerCase(PIEGE);
                        }
                    }
                }
            }
        }
};


int main(){
    return 0;
}
