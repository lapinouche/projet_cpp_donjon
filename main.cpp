// ERREUR LIGNE 206 ; 236

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // for random
#include <queue>
#include <utility> // for pair
#include <typeinfo>

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
    ENTREE,
    SORTIE
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
        /*
        // to be done later
        void boucleDeJeu(Donjon& d){
        }*/
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
                default:{
                    return nullptr;
                }
            }
        }
};

class Donjon {
    protected: // protected, like this Aventurier can see it
        vector<vector<Case*>> grille;
        int largeur;
        int hauteur;

    public:
        Donjon() : largeur(0), hauteur(0) {}
        Donjon(const vector<vector<Case*>>& g, const int l=0, const int h=0) : largeur(l), hauteur(h), grille(g) {}

        void afficher(){
            for (int j=0; j < hauteur; j++){
                for (int i=0; i < largeur; i++){
                    cout << grille[i][j]->afficher();
                }
                cout << endl;
            }
        }

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
        } //return grille;


        void poserEntree() {
            delete grille[1][1];
            grille[1][1] = new Entree(); 
        }

        void poserSortie() {
            // Note: Use largeur-2/hauteur-2 if the edges are walls!
            delete grille[largeur-2][hauteur-2];
            grille[largeur-2][hauteur-2] = new Sortie();
        }

        /*
        Case* poserEntree(){ // const vector<vector<Case*>>& grille
            Case* entree = grille[1][1]; //grille[0][0];
            return entree;
        }

        Case* poserSortie(){ // const vector<vector<Case*>>& grille
            Case* sortie = grille[(largeur-1)][(hauteur-1)];
            return sortie;
        } */

        void initialiserGrille(int largeur, int hauteur){ // void ou vector<vector<Case*>> !?
            generer(largeur, hauteur);
            genererLabyrinthe(1, 1);
            poserEntree();
            poserSortie();
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
};

class BFS : public Donjon {
    private:
        queue<pair<int, int>> file;
        vector<vector<pair<int, int>>> parent;

    public:
        BFS() = default; // tells the compiler to use the default empty state for both containers
        BFS(vector<vector<Case*>>& grille, const queue<pair<int, int>> f, const vector<vector<pair<int, int>>>& p) : Donjon(grille), file(f), parent(p) {}

        void setDonjonData(const vector<vector<Case*>>& g, int l, int h) {
            this->grille = g;
            this->largeur = l;
            this->hauteur = h;
        }
        
        queue<pair<int, int>> trouverChemin(pair<int, int> depart = {1, 1}, pair<int, int> arrivee = {5, 0}){
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
                        if (!visite[nx][ny] && grille[nx][ny]->getType() != TypeCase::MUR) {
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

            chemin.front() = depart;
            return chemin;
        }
};

// https://www.pointerlab.fr/blog/cpp-std-queue

class Aventurier : public Case, Donjon {
    private:
        pair<int, int> last_pos; // à mettre à jour lors avant le deplacement du joueur dans la boucle de jeu
        pair<int, int> position;
        int sante; // point de vie du joueur (initialiser à 100/100)
        int inventaire; // nombre de tresor ramasser par le joueur
    
    public:
        Aventurier(vector<vector<Case*>>& grille, const pair<int, int>& last_pos = {0, 0}, const pair<int, int>& position = {0, 0}, const int sante = 100, const int inventaire = 0) : Donjon(grille), last_pos(last_pos), position(position), sante(sante), inventaire(inventaire) {}

        char afficher () override {
            return '@';
        }

        void deplacer(int nx, int ny){
            // precondition : la case (nx, ny) est franchissable (pas un mur)
            if (Donjon::casevalide(nx, ny) == true){
                position = {nx, ny};
            }
        }

        bool estVivant(){
            if (sante > 0){
                return true;
            }
            return false;
        }

        void resoudreCase(Case* c){
            if (typeid(c) == typeid(Monstre)){
                string choix = "fuire";
                cout << "Voulez vous combatre ou fuire ?" << endl;
                cin >> choix;
                if (choix == "combatre"){
                    int r = rand() % 100;
                    if (r < 50){ // valeur choisi arbitrairement 
                        cout << "Combat perdu";
                        sante -= 40; // valeur choisi arbitrairement 
                    }
                    else{
                        cout << "Combat gagné";
                        sante += 20; // valeur choisi arbitrairement 
                    }
                    
                }
                else if (choix == "fuire"){
                    position = last_pos;
                }
                else{
                    cout << "saisie incorrect" << endl;
                    resoudreCase(c);
                }
            }
            else if (typeid(c) == typeid(Tresor)){
                inventaire += 1;
                grille[position.first][position.second] =  CaseFactory::creerCase(TypeCase::PASSAGE);
            }
            else if (typeid(c) == typeid(Piege)){
                inventaire -= 1; // valeur choisi arbitrairement
            }
        }

        void afficherStatut(){
            cout << "position courante : ";
            cout << "(" << position.first << ", " << position.second << ")" << endl;
            cout << "points de vie : " << sante << endl;
            cout << "contenu de l'inventaire : " << inventaire << endl;
        }
};

int main(){
    srand(time(NULL));
    Donjon d;
    d.initialiserGrille(21, 11);
    d.afficher();
    
    BFS bfs;
    bfs.initialiserGrille(21, 11);
    queue<pair<int, int>> chemin = bfs.trouverChemin({1, 1}, {19, 9}); // {0, 0}, {21, 11}  --  {grille[0].size(), grille.size()}
    
    if(!chemin.empty()) {
        cout << "Path found! Length: " << chemin.size() << endl;
    } else {
        cout << "No path found." << endl;
    }

    return 0;
}
