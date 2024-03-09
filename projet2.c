#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "projet2.h"
int main() {
    creerFichierAdminEtudiant();
    initialiserEtudiantsL1(listeL1, &nb_etudiants_L1);
    initialiserEtudiantsL2(listeL2, &nb_etudiants_L2);
    interfaceConnexion();
    return 0;
}