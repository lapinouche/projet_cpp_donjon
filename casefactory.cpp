#include <iostream>
#include "case.cpp"

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
            throw std::invalid_argument("Type de case inconnu");
        }
    }
};