#include <iostream>
#include <vector>
#include <string>
#include <cstdlib> // for random
#include <queue>
#include <utility> // for pair
#include <typeinfo>

// Non utiliser pour le moment
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
    PIEGE
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
        int getValeur () const { return valeur ; }
};

class Monstre : public Case {
    private :
        int pv ;
    public :
        Monstre (int p = 20, const bool& visiter = false) : pv(p), Case(visiter) {}
        TypeCase getType() const override { return TypeCase::MONSTRE; }
        char afficher () override { return 'M'; }
        int getPv () const { return pv ; }
};

class Piege : public Case {
    private :
        int degats ;

    public :
        Piege (int d = 15, const bool& visiter = false) : degats(d), Case(visiter) {}
        TypeCase getType() const override { return TypeCase::PIEGE; }
        char afficher () override { return 'T'; }
        int getDegats () const { return degats; }
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

        void melanger(vector<string> directions){ // à modifier
            for (int i = directions.size() - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                swap(directions[i], directions[j]);
            }
        }

        void generer(int largeur, int hauteur){
            this->largeur = largeur;
            this->hauteur = hauteur;
            grille.assign(largeur, vector<Case*>(hauteur, nullptr));
            //vector<vector<Case*>> newgrille(largeur, vector<Case*>(hauteur, nullptr));
            //this->grille = newgrille;
            
            for (int i=0; i < largeur; i++){
                for (int j=0; j < hauteur; j++){
                    grille[i][j] = CaseFactory::creerCase(TypeCase::MUR);
                }
            }
        }

        void genererLabyrinthe(int x, int y){ 
            grille[x][y]->SetV(true); // error : Assertion '__n < this->size()' failed.
            vector<string> directions = {"NORD", "SUD", "EST", "OUEST"};
            melanger(directions);

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
                /*bool v = grille[nx][ny]->Case::getV();*/ //  error : Assertion '__n < this->size()' failed.
                
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

        Case* poserEntree(){ // const vector<vector<Case*>>& grille
            Case* entree = grille[1][1]; //grille[0][0];
            return entree;
        }

        Case* poserSortie(){ // const vector<vector<Case*>>& grille
            Case* sortie = grille[(largeur-1)][(hauteur-1)];
            return sortie;
        }

        void initialiserGrille(int largeur, int hauteur){ // void ou vector<vector<Case*>> !?
            generer(largeur, hauteur);
            poserEntree();
            poserSortie();
            genererLabyrinthe(1, 1);
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

        bool casevalide(int x, int y){
            if (x < 0 || x >= largeur || y < 0 || y >= hauteur ){
                return false;
            }
            return grille[x][y]->getType() != TypeCase::MUR;
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

class Aventurier : public Case, Donjon {
    private:
        vector<vector<Case*>> grille;
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
    
    //d.placerElement();
    
    d.afficher();
    /*
    BFS bfs;
    bfs.trouverChemin(grille, {0, 0}, {grille[0].size(), grille.size()});
    */
    return 0;
}
