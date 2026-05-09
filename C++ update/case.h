#ifndef CASE_H
#define CASE_H
#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <iostream>
using namespace std;

enum class TypeCase {
    MUR,
    PASSAGE,
    TRESOR,
    MONSTRE,
    PIEGE, 
    ENTREE,
    SORTIE,
    PELLE
    };

class Case{
protected:
    bool visiter;

public:
    Case(const bool& visiter = false) : visiter(visiter) {}
    virtual ~Case() = default;
    virtual string afficher() const = 0;
    virtual TypeCase getType() const = 0;
    virtual int getEffect() const { return 0; }
    
    void SetV(bool newstate){ 
        this->visiter = newstate; 
    }

    bool getV() const{ 
        return visiter; 
    }

    friend ostream& operator<<(ostream& os, const Case* c) {
        if(c) os << c->afficher();
        return os;
    }
};

class Mur : public Case {
public :
    Mur (const bool& visiter = false) : Case(visiter) {}
    virtual TypeCase getType() const override { return TypeCase::MUR; }

    string afficher() const override { 
        return "🧱" ; 
    }
};


class Passage : public Case {
public : 
    Passage(const bool& visiter = false) : Case(visiter) {}
    virtual TypeCase getType() const override { return TypeCase::PASSAGE; }

    string afficher() const override { 
        return "  " ; 
    }
};

class Tresor : public Case {
private :
    int valeur ;

public :
    Tresor (int v = 10, const bool& visiter = false) : Case(visiter), valeur(v) {}
    virtual TypeCase getType() const override { return TypeCase::TRESOR; }
    int getEffect() const override { return valeur; }

    string afficher() const override { 
        return "💰"; 
    }

    void setValeur (int valeur) { 
        this->valeur = valeur; 
    }

    int getValeur () const { 
        return valeur ; 
    }
};

class Monstre : public Case {
private :
    int pv ;
    mutable int frame = 0;

public :
    Monstre (int p = 20, const bool& visiter = false) :  Case(visiter), pv(p) {}
    virtual TypeCase getType() const override { return TypeCase::MONSTRE; }
    int getEffect() const override { return pv; }

    string afficher() const override {
        frame++;
        // Alterne entre deux états tous les 2 rafraîchissements
        if ((frame / 2) % 2 == 0) return "👾"; 
        else return "👽"; 
    }

    void setPv (int pv) { 
        this->pv = pv; 
    }

    int getPv () const { 
        return pv ; 
    }
};

class Piege : public Case {
private :
    int degats ;

public :
    Piege (int d = 100, const bool& visiter = false) : Case(visiter), degats(d) {}
    virtual TypeCase getType() const override { return TypeCase::PIEGE; }
    int getEffect() const override { return degats; }

    string afficher() const override { 
        return "💣"; 
    }

    void setDegats (int degats) { 
        this->degats = degats; 
    }

    int getDegats () const { 
        return degats; 
    }
};

class Entree : public Case {
public:
    virtual TypeCase getType() const override { return TypeCase::ENTREE; }
    string afficher() const override { 
        return "🎬"; 
    }
};

class Sortie : public Case {
public:
    virtual TypeCase getType() const override { return TypeCase::SORTIE; }
    string afficher() const override { 
        return "🏁"; 
    }
};

class Pelle : public Case {
public:
    virtual TypeCase getType() const override { return TypeCase::PELLE; }
    string afficher() const override { 
        return "🥄"; 
    }
};

class CaseFactory {
public:

    static Case* creerCase(TypeCase type) {
        switch (type) {
            case TypeCase::MUR:     return new Mur();
            case TypeCase::PASSAGE: return new Passage();
            case TypeCase::TRESOR:  return new Tresor();
            case TypeCase::MONSTRE: return new Monstre();
            case TypeCase::PIEGE:   return new Piege();
            case TypeCase::ENTREE:  return new Entree();
            case TypeCase::SORTIE:  return new Sortie();
            case TypeCase::PELLE:   return new Pelle();
            default:                return nullptr;
        }
    }
};

#endif

