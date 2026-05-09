#ifndef DONJON_H
#define DONJON_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <iostream>
using namespace std;

#include "case.h"

using namespace std;

class Donjon {
private:
    int largeur;
    int hauteur;
    vector<vector<Case*>> grille;
    pair<int, int> entree;
    pair<int, int> sortie;
    
public:
    Donjon() : largeur(0), hauteur(0) {
        srand(time(NULL));
    }

    ~Donjon() {
        for (int i = 0; i < largeur; i++) {
            for (int j = 0; j < hauteur; j++) {
                delete grille[i][j];
            }
        }
    }

    void melanger(vector<string>& directions){
        for (int i = directions.size() - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            swap(directions[i], directions[j]);
        }
    }

    void genererLabyrinthe(int x, int y){ 
        grille[x][y]->SetV(true); // error : Assertion '__n < this->size()' failed.
        vector<string> directions = {"NORD", "SUD", "EST", "OUEST"};
        melanger(directions);

        for (string d : directions){
            int nx, ny, mx, my;

            if (d == "NORD"){
                nx = x; ny = y + 2; mx = x; my = y + 1;
            }
            if (d == "SUD"){
                nx = x; ny = y - 2; mx = x; my = y - 1; 
            }
            if (d == "EST"){
                nx = x + 2; ny = y; mx = x + 1; my = y;
            }
            if (d == "OUEST"){
                nx = x - 2; ny = y; mx = x - 1; my = y;
            }

            bool boundsCheck = (nx > 0 && nx < largeur - 1 && ny > 0 && ny < hauteur - 1);
            
            if (boundsCheck){ 
                if (grille[nx][ny] != nullptr) {
                    if (grille[nx][ny] -> getType() == TypeCase::MUR){
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
    }

    void generer(int l, int h){
        largeur = l;
        hauteur = h;
        grille.assign(largeur, vector<Case*>(hauteur, nullptr));
        
        for (int i = 0; i < largeur; i++){
            for (int j = 0; j < hauteur; j++){
                grille[i][j] = CaseFactory::creerCase(TypeCase::MUR);
            }
        }

        genererLabyrinthe(1, 1);
        poserEntree();
        poserSortie();
        placerElement();
    }

    void poserEntree() {
        delete grille[1][1];
        grille[1][1] = CaseFactory::creerCase(TypeCase::ENTREE); 
    }

    void poserSortie() {
        // Note: Use largeur-2/hauteur-2 if the edges are walls!
        int sx = largeur - 2;
        int sy = hauteur - 2;
        delete grille[sx][sy];
        grille[sx][sy] = CaseFactory::creerCase(TypeCase::SORTIE);
        sortie = {sx, sy};
    }

    void placerElement(){ // vector<vector<Case*>>& grille
        for (int i = 0; i < largeur; i++){
            for (int j = 0; j < hauteur; j++){
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
                    else if (r < 15) {
                        delete grille[i][j];
                        grille[i][j] = CaseFactory::creerCase(TypeCase::PELLE);
                    }
                }
            }
        }
    }

    void afficher(int posx, int posy) {
        cout << "🏰"; 
        for(int i = 1; i < largeur - 2; i++) cout << "──";
        cout << "🏰" << endl;

        for (int j = 1; j < hauteur - 1; j++){
            cout << "┃";
            for (int i = 1; i < largeur - 1; i++){
                if (i == posx && j == posy) cout << "👤";
                else cout << grille[i][j];
            }
            cout << "┃" << endl;
        }
        cout << "🏰"; 
        for(int i = 1; i < largeur - 2; i++) cout << "──";
        cout << "🏰" << endl;
        cout << "\n" << endl;
    }

    pair<int,int> getEntree() { return entree; }
    pair<int,int> getSortie() { return sortie; }
    Case* getGrille(int i, int j) { return grille[i][j]; }
    Case* getCase(int i, int j) {
        if (i >= 0 && i < largeur && j >= 0 && j < hauteur) {
            return grille[i][j];
        }
        return nullptr; // Hors limites
    }
    void setCase(int i, int j, TypeCase type) {
        delete grille[i][j];
        grille[i][j] = CaseFactory::creerCase(type);
    }
    int getLargeur() { return largeur; }
    int getHauteur() { return hauteur; }


    vector<pair<int, int>> trouverChemin(pair<int, int> depart, pair<int, int> arrivee){
        queue<pair<int, int>> file;
        vector<vector<bool>> visite(largeur, vector<bool>(hauteur, false));
        vector<vector<pair<int, int>>> parent(largeur, vector<pair<int, int>>(hauteur, {-1, -1}));
    
        file.push(depart); //enfiler(file, depart)
        visite[depart.first][depart.second] = true;
        
        while(!file.empty()){
            pair<int, int> courant = file.front();
            file.pop();

            if (courant == arrivee) return reconstruireChemin(parent, depart, arrivee);

            int dx[] = {0, 0, 1, -1};
            int dy[] = {1, -1, 0, 0};

            for (int i = 0; i < 4; i++) {
                int nx = courant.first + dx[i];
                int ny = courant.second + dy[i];

                // PROPER BOUNDS CHECK
                if (nx >= 0 && nx < largeur && ny >= 0 && ny < hauteur) {
                    if (grille[nx][ny] != nullptr) {
                        if (!visite[nx][ny] && grille[nx][ny]->getType() != TypeCase::MUR) {
                            visite[nx][ny] = true;
                            parent[nx][ny] = courant;
                            file.push({nx,ny});
                        }

                    }
                }
            }
        }
        return {};
    }

    vector<pair<int, int>> reconstruireChemin(const vector<vector<pair<int, int>>>& parent, pair<int, int> depart, pair<int, int> arrivee){
        vector<pair<int, int>> chemin;
        pair<int, int> courant = arrivee;

        while (courant != depart){
            chemin.push_back(courant);
            courant = parent[courant.first][courant.second];
        }
        chemin.push_back(depart);
        reverse(chemin.begin(), chemin.end());

        return chemin;
    }

};

#endif