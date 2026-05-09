#ifndef AVENTURIER_H
#define AVENTURIER_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <iostream>
#include <string>

#ifdef _WIN32
    #include <conio.h>    // Pour Windows (getch et kbhit sont déjà dedans)
    #include <windows.h>  // Pour Sleep()
#else
    #include <termios.h>  // Pour Linux
    #include <unistd.h>   // Pour usleep()
    #include <cstdio>

    #include <fcntl.h>

    int kbhit(void) {
        struct termios oldt, newt;
        int ch;
        int oldf;

        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

        ch = getchar();

        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);

        if(ch != EOF) {
            ungetc(ch, stdin);
            return 1;
        }

        return 0;
    }

    char getch() {
        char touche = 0;
        struct termios configuration;
        
        tcgetattr(0, &configuration);            // 1. Lire la config actuelle
        configuration.c_lflag &= ~ICANON;       // 2. Désactiver la touche "Entrée"
        configuration.c_lflag &= ~ECHO;         // 3. Désactiver l'affichage des touches
        tcsetattr(0, TCSANOW, &configuration);  // 4. Appliquer
        
        read(0, &touche, 1);                    // 5. Attendre et lire UNE touche
        
        configuration.c_lflag |= ICANON;        // 6. Rallumer "Entrée"
        configuration.c_lflag |= ECHO;          // 7. Rallumer l'affichage
        tcsetattr(0, TCSANOW, &configuration);  // 8. Réappliquer
        
        return touche;
    }


#endif

using namespace std;


#include "case.h"
#include "donjon.h"

class Aventurier{
private:
    int x, y;
    int sante, santeMax; // point de vie du joueur (initialiser à 100/100)
    int inventaire; // nombre de tresor ramasser par le joueur
    int pelle; // nombre de pelle dans l'inventaire
    string journal;

public:
    Aventurier(int X, int Y, int p = 100) 
    : x(X), y(Y), sante(p), santeMax(p), inventaire(0), pelle(0), journal(""){}

    int getX() { return x; }
    int getY() { return y; }

    void deplacer(int nx, int ny) {
        x = nx;
        y = ny;
    }

    void resoudreCase(Case* c, int nx, int ny, Donjon& d) {
        if (c == nullptr) {
            journal = "Case hors limites !" ;
            return;
        }
        
        if (c->getType() == TypeCase::TRESOR) {
            inventaire++;
            journal = "Trésor trouvé !";
            d.setCase(nx, ny, TypeCase::PASSAGE);
            deplacer(nx, ny);
            return;
        }

        if (c -> getType() == TypeCase::PIEGE){
            int degats = c->getEffect();
            sante -= degats;
            journal = "Tu es tombé dans un piège. -" + to_string(degats) + " PV";
            d.setCase(nx, ny, TypeCase::PASSAGE);
            deplacer(nx, ny);
            return;
        }

        if (c -> getType() == TypeCase::MONSTRE){
            cout << "Un monstre ! (c)ombat ou (f)uite ? + Entrée" << endl;
            char choix; cin >> choix;
            if (choix == 'c' || choix == 'C'){
                int degats = c->getEffect() / 2;
                sante -= degats;
                journal = "Victoire ! -" + to_string(degats) + " PV";
                d.setCase(nx, ny, TypeCase::PASSAGE);
                deplacer(nx, ny);
            } else if (choix == 'f' || choix == 'F'){
                journal : "Vous avez pris la fuite !";
            } else {
                journal = "Saisie incorrect";
            }
            return;
        }
        
        if (c -> getType() == TypeCase::SORTIE){
            deplacer(nx, ny);
            journal = "Félicitations ! Vous avez atteint la sortie !";
            return;
        }

        if (c -> getType() == TypeCase::MUR){
           journal = "Vous ne pouvez pas aller par là !";
            return;
        }

        if (c -> getType() == TypeCase::PELLE){
            journal = "Vous avez trouvé une pelle ! Vous pouvez maintenant creuser à côté de vous.";
            pelle++;
            d.setCase(nx, ny, TypeCase::PASSAGE);
            deplacer(nx, ny);
            return;
        }

        else deplacer(nx, ny);
    }

    bool estVivant() { return sante > 0; }

    void afficherStatut(Donjon& d) const{
        vector<pair<int, int>> chemin = d.trouverChemin({x, y}, d.getSortie());
        if (!journal.empty()) {
            cout << journal << endl;
        }
        cout << "Position : (" << x << ", " << y << ")" << endl;
        cout << "Santé : " << sante << "/" << santeMax << endl;
        cout << "Inventaire : " << inventaire << " trésors, " << pelle << " pelles" << endl;
        if (chemin.empty()) cout << "Distance à la sortie : ? cases" << endl;
        else cout << "Distance à la sortie : " << chemin.size() - 1 << " cases" << endl;
        cout << "Pelle : " << pelle << endl;
    }

    void reset() {
        x = 1;
        y = 1;
        sante = 100;
        pelle = 0;
        inventaire = 0;
        estVivant();
        journal = "Nouvelle partie commence !";
    }

    void afficherMenu(Donjon& d) {
        int ligne = 1;
        
        while (true) {
            system("clear");

            cout << "\n\n";
            cout << "  ██████╗  ██████╗ ███╗   ██╗     ██╗ ██████╗ ███╗   ██╗" << endl;
            cout << "  ██╔══██╗██╔═══██╗████╗  ██║     ██║██╔═══██╗████╗  ██║" << endl;
            cout << "  ██║  ██║██║   ██║██╔██╗ ██║     ██║██║   ██║██╔██╗ ██║" << endl;
            cout << "  ██║  ██║██║   ██║██║╚██╗██║██   ██║██║   ██║██║╚██╗██║" << endl;
            cout << "  ██████╔╝╚██████╔╝██║ ╚████║╚██████╔╝██████╔╝██║ ╚████║" << endl;
            cout << "  ╚═════╝  ╚═════╝ ╚═╝  ╚═══╝ ╚═════  ╚═════╝ ╚═╝  ╚═══╝" << endl;
            cout << "               --- L'AVENTURE ÉPIQUE ---" << endl;
            cout << "\n\n";

            // 2. OPTIONS CENTRÉES AVEC EMOJIS
            if (ligne == 1) {
                cout << "           🔥  👉  [LANCER LA PARTIE]  👈  🔥" << endl;
            } else {
                cout << "                    Lancer la partie             " << endl;
            }

            cout << "\n"; // Un peu d'espace entre les boutons

            if (ligne == 2) {
                cout << "            💀  👉  [QUITTER LE JEU]  👈  💀" << endl;
            } else {
                cout << "                     Quitter le jeu               " << endl;
            }

            cout << "\n\n";
            cout << "    (Utilisez Z et S pour naviguer, Entrée pour valider)" << endl;

            // 3. LOGIQUE
            char touche = getch(); 

            if (touche == 'z' || touche == 'Z') ligne = 1;
            else if (touche == 's' || touche == 'S') ligne = 2;
            else if (touche == 10 || touche == 13) {
                if (ligne == 1) {
                    // Petite animation de chargement avant de lancer
                    cout << "\n              Préparation du donjon... ⚔️" << endl;
                    usleep(1000000);
                    reset(); // Réinitialise les stats de l'aventurier
                    boucleDeJeu(d);
                } else {
                    system("clear");
                    return; 
                }
            }
        }
    }

    void boucleDeJeu(Donjon& d) {
        bool gagne = false;
        while (estVivant() && !gagne) {
            system("clear"); // Pour nettoyer la console à chaque tour (optionnel)
    
            // 1. Affichage du Donjon et des statistiques 
            // On utilise la fonction afficher que tu as déjà faite
            d.afficher(x, y); // Affiche le donjon avec la position de l'aventurier
            afficherStatut(d); // Affiche les stats de l'aventurier
            usleep(100000);

            // 2. Lecture du clavier (z, q, s, d) [cite: 205, 226]
            cout << "\nDirection (z/q/s/d) ou Quitter (e): " << endl;
            char touche = getch();


            journal = "";

            if (touche == 'e' || touche == 'E') break; // Quitter proprement

            // 3. Calcul de la position où on veut aller (nx, ny)
            int nx = x; 
            int ny = y;

            if (touche == 'z' || touche == 'Z') ny--;      // Haut
            else if (touche == 's' || touche == 'S') ny++; // Bas
            else if (touche == 'q' || touche == 'Q') nx--; // Gauche
            else if (touche == 'd' || touche == 'D') nx++; // Droite
            else {
                journal = "Touche inconnue, recommencez.";
                continue; // Recommence la boucle au début
            }

            // 4. Vérification : est-ce un mur ? [cite: 206, 215]
            Case* caseCible = d.getCase(nx, ny);
            
            if (caseCible != nullptr && caseCible->getType() == TypeCase::MUR) {
                if (pelle > 0) {
                    char choix;
                    cout << "Mur détecté ! Voulez-vous utiliser une pelle pour creuser ? (o/n) + Entrée : " << endl;
                    cin >> choix;
                    if (choix == 'o' || choix == 'O') {
                        // On utilise d.setCase pour remplacer le mur par un passage proprement
                        d.setCase(nx, ny, TypeCase::PASSAGE); 
                        pelle--;
                        deplacer(nx, ny);
                        journal =  "Vous avez creusé un tunnel !";
                    } 
                } else {
                    journal = "C'est un mur ! Vous ne pouvez pas passer.";
                }
            } else {
                resoudreCase(caseCible, nx, ny, d);

                // On vérifie après coup si la nouvelle position est la sortie
                if (d.getCase(x, y)->getType() == TypeCase::SORTIE) {
                    gagne = true;
                }
            }
        }

        // 6. Message de fin selon le résultat [cite: 210, 224]
        if (!estVivant()) {
            system("clear");
            cout << "\n\n";
            cout << "        /\\________________________/\\" << endl;
            cout << "       |🔥  ÉCHEC DE LA MISSION   🔥|" << endl;
            cout << "        \\/________________________\\/" << endl;
            cout << "\n";
            cout << "        💔   VOUS AVEZ SUCCOMBÉ   💔" << endl;
            cout << "\n";
            cout << "[ Les trésors ont été perdus dans les abysses ]" << endl;
            cout << "[         Votre aventure s'arrête ici         ]" << endl;
            cout << "\n";
            cout << "  __________________________________________" << endl;
        }

        else if (gagne) {
            system("clear");
            cout << "\n\n";
            cout << "      🏆✨🏆✨🏆✨🏆✨🏆✨🏆✨🏆✨🏆" << endl;
            cout << "      ✨                           ✨" << endl;
            cout << "      🏆      NIVEAU TERMINÉ       🏆" << endl;
            cout << "      ✨                           ✨" << endl;
            cout << "      🏆✨🏆✨🏆✨🏆✨🏆✨🏆✨🏆✨🏆" << endl;
            cout << "\n";
            cout << "      🎊  FÉLICITATIONS AVENTURIER  🎊" << endl;
            cout << "\n";
            cout << "      Vous sortez du donjon la tête haute." << endl;
            cout << "        Le trésor est enfin à vous ! 💰" << endl;
        }

        cout << "\n  Appuyer sur une touche pour revenir au menu..." << endl;
        getch(); // Plus simple que cin.ignore/get !
    }
};

#endif // AVENTURIER_H