#include <iostream>
#include "case.cpp"

class CaseFactory {
public:
    static Case* creerCase(char type) {
        if (type == '#') {
            return new Mur();
        } else if (type == ' ') {
            return new Passage();
        } else if (type == '+') {
            return new Tresor();
        } else if (type == 'M') {
            return new Monstre();
        } else if (type == 'T') {
            return new Piege();
        } else {
            throw std::invalid_argument("Type de case inconnu");
        }
    }
};