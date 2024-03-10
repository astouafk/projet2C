#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "projet2.h"
int main() {
    creerFichierAdminEtudiant();
    initialiserEtudiantsL1(listeL1, &nb_etudiants_L1);
    initialiserEtudiantsL2(listeL2, &nb_etudiants_L2);
    interfaceConnexion();
    Utilisateur nouvelEtudiant;
    Utilisateur etudiant;
    
    etudiant.messages_non_lus = 2; // Exemple, à remplacer par la vraie récupération
    
    
    return 0;
}