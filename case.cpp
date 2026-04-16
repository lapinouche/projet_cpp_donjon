#include <iostream> 
#include <vector>
// Classe abstraite de base
class Case {
public :

    virtual char afficher () = 0;
    virtual ~Case () = default ;

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
