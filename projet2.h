#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>



#ifdef _WIN32
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

#define MAX_BUFFER 50
#define MAX_STUDENTS 10
typedef struct {
    char matricule[MAX_BUFFER];
    char motdepasse[MAX_BUFFER];
    int typeUtilisateur;
    int etatUtilisateur;
} Utilisateur;

typedef struct {
    char matricule[MAX_BUFFER];
    char prenom[MAX_BUFFER];
    char nom[MAX_BUFFER];
    char dateNaissance[MAX_BUFFER];
    char motDePasse[MAX_BUFFER];
    char classe[MAX_BUFFER];
    int nbrAbsence;
    int cumulRetard;
    int presence;
} Etudiant;

int nb_etudiants_L1 = 0;
int nb_etudiants_L2 = 0;
Etudiant listeL1[MAX_STUDENTS];
Etudiant listeL2[MAX_STUDENTS];

char getch();
int verifierIdentifiants(const char *matricule, const char *motdepasse, int *typeUtilisateur, int *etatUtilisateur);
void getHiddenInput(char *password, int maxLength);
Utilisateur interfaceConnexion();
void creerFichierAdminEtudiant();
void afficherMenuAdmin();
void afficherMenuEtudiant(Utilisateur interface);
void initialiserEtudiantsL1(Etudiant liste[], int *nb_etudiants);
void initialiserEtudiantsL2(Etudiant liste[], int *nb_etudiants);
int presenceEnregistreeAujourdhui(const char *matricule);
void marquerPresence(Etudiant listeL1[], int nb_etudiantsL1, Etudiant listeL2[], int nb_etudiantsL2);
void marquerPresenceEtudiant(Etudiant listeL1[], int nb_etudiantsL1, Etudiant listeL2[], int nb_etudiantsL2, Utilisateur etudiant);
void genererListePresence();
void genererFichierPresenceParDate();



char getch() {
#ifdef _WIN32
    return _getch();
#else
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
#endif
}

int verifierIdentifiants(const char *matricule, const char *motdepasse, int *typeUtilisateur, int *etatUtilisateur) {
    FILE *fichier;
    fichier = fopen("logadminetu.txt", "r");
    if (fichier != NULL) {
        Utilisateur utilisateur;
        while (fscanf(fichier, "%s %s %d %d", utilisateur.matricule, utilisateur.motdepasse, &utilisateur.typeUtilisateur, &utilisateur.etatUtilisateur) == 4) {
            if (strcmp(matricule, utilisateur.matricule) == 0 && strcmp(motdepasse, utilisateur.motdepasse) == 0) {
                *typeUtilisateur = utilisateur.typeUtilisateur;
                *etatUtilisateur = utilisateur.etatUtilisateur;
                fclose(fichier);
                return 1; // Connexion réussie
            }
        }
        fclose(fichier);
    }
    return 0; // Identifiants incorrects
}

void getHiddenInput(char *password, int maxLength) {
    int i = 0;
    char ch;
    while (1) {
        ch = getch();
        if (ch == '\n' || ch == '\r') {
            password[i] = '\0';
            break;
        } else if (ch == 127 && i > 0) {
            printf("\b \b");
            i--;
        } else if (isprint(ch) && i < maxLength - 1) {
            password[i++] = ch;
            printf("*");
        }
    }
}

Utilisateur interfaceConnexion() {
    Utilisateur interface;

    char matricule[MAX_BUFFER];
    char motdepasse[MAX_BUFFER];
    int typeUtilisateur, etatUtilisateur;
    Utilisateur stock;
    
    while (1) {
        printf("\nConnexion :\n");
        
        while (1) {
            printf("Entrer votre username : ");
            if (fgets(matricule, sizeof(matricule), stdin) == NULL) {
                continue;
            }
            matricule[strcspn(matricule, "\n")] = '\0';
            
            if (strlen(matricule) == 0) {
                printf("Nom d'utilisateur ne peut pas être vide. Veuillez ressaisir.\n");
            } else if (isspace(matricule[0])) {
                printf("Nom d'utilisateur invalide. Veuillez ressaisir.\n");
            } else {
                break; 
            }
        }
        
        while (1) {
            printf("Entrer votre mot de passe : ");
            getHiddenInput(motdepasse, MAX_BUFFER);
            if (isspace(motdepasse[0])) {
                printf("\nMot de passe invalide. Veuillez ressaisir.\n");
            } else {
                break; 
            }
        }

        strcpy(stock.matricule, matricule);
        strcpy(stock.motdepasse, motdepasse);

        if (verifierIdentifiants(stock.matricule, stock.motdepasse, &typeUtilisateur, &etatUtilisateur)) {
            printf("\nConnexion réussie !\n");
            // Initialiser la variable interface avec les informations pertinentes
            strcpy(interface.matricule, matricule);
            strcpy(interface.motdepasse, motdepasse);
            interface.typeUtilisateur = typeUtilisateur;
            interface.etatUtilisateur = etatUtilisateur;

            if (typeUtilisateur == 1 && etatUtilisateur == 1)
                afficherMenuAdmin();
            else if (typeUtilisateur == 2 && etatUtilisateur == 1)
                afficherMenuEtudiant(interface);
            break; 
        } else {
            printf("Échec de la connexion. Identifiants incorrects.\n");
        }
    }

    return interface;
}


void creerFichierAdminEtudiant() {
    FILE *fichier;
    fichier = fopen("logadminetu.txt", "w+");

    if (fichier != NULL) {
        fprintf(fichier, "admin passer123 1 1\n");
        fprintf(fichier, "admin2 passer456 1 1\n");
        for (int i = 1; i <= 3; i++) {
            for (int j = 1; j <= 10; j++) {
                fprintf(fichier, "etudiant%dL%d pass%dl%d 2 1\n", j, i, j, i);
            }
        }
        fclose(fichier);
    } else {
        printf("Impossible de créer le fichier logadminetu.txt.\n");
    }
}
void genererListePresence() {
    FILE *fichierPresence = fopen("presence.txt", "r");
    FILE *fichierListePresence = fopen("listepresence.txt", "w");

    if (fichierPresence == NULL || fichierListePresence == NULL) {
        printf("Erreur lors de l'ouverture des fichiers.\n");
        return;
    }

    typedef struct {
        char matricule[MAX_BUFFER];
        char nom[MAX_BUFFER];
        char prenom[MAX_BUFFER];
        char classe[MAX_BUFFER];
        char date[MAX_BUFFER];
        char heure[MAX_BUFFER];
    } EtudiantPresent;

    char ligne[MAX_BUFFER];
    char dateActuelle[MAX_BUFFER] = "";
    EtudiantPresent etudiants[MAX_STUDENTS];
    int nbEtudiants = 0;

    while (fgets(ligne, sizeof(ligne), fichierPresence) != NULL) {
        char matricule[MAX_BUFFER], nom[MAX_BUFFER], prenom[MAX_BUFFER], classe[MAX_BUFFER], date[MAX_BUFFER], heure[MAX_BUFFER];
        sscanf(ligne, "%s %s %s %s %s %s", matricule, nom, prenom, classe, date, heure);

        if (strcmp(date, dateActuelle) != 0) {
            if (nbEtudiants > 0) {
                // Formater la date au format français jj/mm/aaaa
                char dateFormatee[MAX_BUFFER];
                char annee[5], mois[3], jour[3];
                strncpy(annee, dateActuelle, 4);
                annee[4] = '\0';
                strncpy(mois, dateActuelle + 5, 2);
                mois[2] = '\0';
                strncpy(jour, dateActuelle + 8, 2);
                jour[2] = '\0';
                sprintf(dateFormatee, "%s/%s/%s", jour, mois, annee);

                // Trier les dates dans l'ordre décroissant
                fprintf(fichierListePresence, "Liste des présences à la date %s\n", dateFormatee);
                fprintf(fichierListePresence, "%-10s | %-20s | %-20s | %-10s | %-20s\n", "Matricule", "Preom", "Nom", "Classe", "Heure de marquage presence");
                for (int i = 0; i < nbEtudiants; i++) {
                    fprintf(fichierListePresence, "%-10s | %-20s | %-20s | %-10s | %-20s\n", etudiants[i].matricule, etudiants[i].nom, etudiants[i].prenom, etudiants[i].classe, etudiants[i].heure);
                }
                fprintf(fichierListePresence, "\n");
            }
            strcpy(dateActuelle, date);
            nbEtudiants = 0;
        }

        strcpy(etudiants[nbEtudiants].matricule, matricule);
        strcpy(etudiants[nbEtudiants].nom, nom);
        strcpy(etudiants[nbEtudiants].prenom, prenom);
        strcpy(etudiants[nbEtudiants].classe, classe);
        strcpy(etudiants[nbEtudiants].date, date);
        strcpy(etudiants[nbEtudiants].heure, heure);
        nbEtudiants++;
    }

    if (nbEtudiants > 0) {
        // Formater la dernière date au format français jj/mm/aaaa
        char dateFormatee[MAX_BUFFER];
        char annee[5], mois[3], jour[3];
        strncpy(annee, dateActuelle, 4);
        annee[4] = '\0';
        strncpy(mois, dateActuelle + 5, 2);
        mois[2] = '\0';
        strncpy(jour, dateActuelle + 8, 2);
        jour[2] = '\0';
        sprintf(dateFormatee, "%s/%s/%s", jour, mois, annee);

        fprintf(fichierListePresence, "Liste des présences à la date %s\n", dateFormatee);
        fprintf(fichierListePresence, "%-10s | %-20s | %-20s | %-10s | %-20s\n", "Matricule", "Prenom", "Nom", "Classe", "Heure de marquage presence");
        for (int i = 0; i < nbEtudiants; i++) {
            fprintf(fichierListePresence, "%-10s | %-20s | %-20s | %-10s | %-20s\n", etudiants[i].matricule, etudiants[i].nom, etudiants[i].prenom, etudiants[i].classe, etudiants[i].heure);
        }
        fprintf(fichierListePresence, "\n");
    }
    

    fclose(fichierPresence);
    fclose(fichierListePresence);

    printf("Le fichier listepresence.txt a été généré avec succès.\n\n");
}



void afficherMenuAdmin() {
    int choix;
    printf("\n");
    printf("***********Bienvenue dans votre Espace ADMIN.***********\n");
    printf("\n");
    do {
        printf("\n1. Gestion des etudiants\n");
        printf("2. Generation de fichier\n");
        printf("3. Marquer les presences\n");
        printf("4. Envoyer un message\n");
        printf("5. Parametres\n");
        printf("6. Quitter\n");
        scanf("%d", &choix);
        if (choix < 1 || choix > 6) {
            printf("Choix invalide\n");
            scanf("%d", &choix);
        }
        switch (choix) {
        case 2:
            int mch;
            do {
                printf("1. Generer la liste des presences par date\n");
                printf("2. La liste des presences pour une date donnee (format date jj/mm/aaaa)\n");
                printf("3. Retour\n");
                scanf("%d", &mch);
                if (mch < 1 || mch > 3) {
                    printf("Choix invalide\n");
                    scanf("%d", &mch);
                }
                switch (mch) {
                case 1:
                    genererListePresence();
                    break;
                case 2:
                    genererFichierPresenceParDate();
                    break;
                case 3:
                    break;
                }
            } while (mch!=3);
            break;
        case 3:
            marquerPresence(listeL1, nb_etudiants_L1, listeL2, nb_etudiants_L2);
            break;
        case 6:
            char ch;
            printf("Voulez-vous vous reconnecter? (o/n) ");
            scanf(" %c", &ch);

            if (ch == 'o' || ch == 'O') {
                while (getchar() != '\n');
                interfaceConnexion();
            } else if (ch == 'n' || ch == 'N') {
                printf("Au revoir !\n");
                exit(0);
            } else {
                printf("Choix invalide.\n");
            }
            break;
        default:
            break;
        }
        
    } while (choix != 6);
}

void afficherMenuEtudiant(Utilisateur interface) {
    int ch;
    printf("\n");
    printf("***********Bienvenue dans votre Espace ETUDIANT.***********\n");
    printf("\n");
    do {
        printf("\n1. Marquer ma presence\n");
        printf("2. Nombre de minutes d'absence\n");
        printf("3. Mes Messages\n");
        printf("4. Quitter\n");
        scanf("%d", &ch);
        if (ch < 1 || ch > 4) {
            printf("Choix invalide\n");
            scanf("%d",&ch);
        }
        if (ch == 4) {
            char choix;
            printf("Voulez-vous vous reconnecter? (o/n) ");
            scanf(" %c", &choix);

            if (choix == 'o' || choix == 'O') {
                while (getchar() != '\n'); 
                interfaceConnexion();
            } else if (choix == 'n' || choix == 'N') {
                printf("Au revoir !\n");
                exit(0);
            } else {
                printf("Choix invalide.\n");
            }
        }
        if (ch == 1) {
             marquerPresenceEtudiant(listeL1, nb_etudiants_L1, listeL2, nb_etudiants_L2, interface);

        }
    } while (ch != 4);
}
void initialiserEtudiantsL1(Etudiant listeL1[], int *nb_etudiants) {
    FILE *fichier = fopen("listeL1.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier listeL1.txt.\n");
        exit(1);
    }

    *nb_etudiants = 0;
    while (fscanf(fichier, "%s %s %s %s %s %s %d %d %d", listeL1[*nb_etudiants].matricule, listeL1[*nb_etudiants].prenom, listeL1[*nb_etudiants].nom, listeL1[*nb_etudiants].dateNaissance, listeL1[*nb_etudiants].motDePasse, listeL1[*nb_etudiants].classe, &listeL1[*nb_etudiants].nbrAbsence, &listeL1[*nb_etudiants].cumulRetard, &listeL1[*nb_etudiants].presence) != EOF && *nb_etudiants < MAX_STUDENTS) {
        (*nb_etudiants)++;
    }

    fclose(fichier);
}

void initialiserEtudiantsL2(Etudiant listeL2[], int *nb_etudiants) {
    FILE *fichier = fopen("listeL2.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier listeL2.txt.\n");
        exit(1);
    }

    *nb_etudiants = 0;
    while (fscanf(fichier, "%s %s %s %s %s %s %d %d %d", listeL2[*nb_etudiants].matricule, listeL2[*nb_etudiants].prenom, listeL2[*nb_etudiants].nom, listeL2[*nb_etudiants].dateNaissance, listeL2[*nb_etudiants].motDePasse, listeL2[*nb_etudiants].classe, &listeL2[*nb_etudiants].nbrAbsence, &listeL2[*nb_etudiants].cumulRetard, &listeL2[*nb_etudiants].presence) != EOF && *nb_etudiants < MAX_STUDENTS) {
        (*nb_etudiants)++;
    }

    fclose(fichier);
}

int presenceEnregistreeAujourdhui(const char *matricule) {
    FILE *fichierPresence = fopen("presence.txt", "r");
    if (fichierPresence == NULL) {
        printf("Erreur lors de l'ouverture du fichier presence.txt pour la vérification.\n");
        return 0; // Indique qu'aucune présence n'est enregistrée pour cet étudiant aujourd'hui
    }

    int presenceTrouvee = 0;
    char ligne[MAX_BUFFER];
    char dateStr[20];
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", localTime);

    while (fgets(ligne, sizeof(ligne), fichierPresence) != NULL) {
        char matriculePresence[MAX_BUFFER], datePresence[20];
        sscanf(ligne, "%s %*s %*s %*s %s", matriculePresence, datePresence);
        if (strcmp(matriculePresence, matricule) == 0 && strcmp(datePresence, dateStr) == 0) {
            presenceTrouvee = 1;
            break;
        }
    }

    fclose(fichierPresence);
    return presenceTrouvee;
}

void marquerPresence(Etudiant listeL1[], int nb_etudiantsL1, Etudiant listeL2[], int nb_etudiantsL2) {
    FILE *fichier;
    char modeOuverture[3] = "a";
    FILE *fichierTest = fopen("presence.txt", "r");

    if (fichierTest == NULL) {
        strcpy(modeOuverture, "w");
    }

    fclose(fichierTest);

    fichier = fopen("presence.txt", modeOuverture);

    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier presence.txt.\n");
        return;
    }
    
    char matricule[MAX_BUFFER];
    char heure[MAX_BUFFER];
    char motDePasseAdmin[MAX_BUFFER];
    int motDePasseValide = 0;
    int etudiantMarquePresent[MAX_STUDENTS] = {0}; // Tableau suivi étudiants marqués présents

    printf("Entrer le matricule de l'étudiant (q pour quitter) : ");
    scanf("%s", matricule);

    while (strcmp(matricule, "q") != 0) {
        // Vérifier si la présence de l'étudiant a déjà été enregistrée aujourd'hui
        if (presenceEnregistreeAujourdhui(matricule)) {
            printf("La présence de l'étudiant a déjà été enregistrée aujourd'hui.\n");
        } else {
            // Procéder à l'enregistrement de la présence
            int etudiantTrouve = 0;
            Etudiant etudiant;
            int dejaPresentAujourdhui = 0;
            int indexEtudiant = -1;

            // Rechercher l'étudiant dans liste L1
            for (int i = 0; i < nb_etudiantsL1; i++) {
                if (strcmp(listeL1[i].matricule, matricule) == 0) {
                    etudiant = listeL1[i];
                    etudiantTrouve = 1;
                    indexEtudiant = i;
                    break;
                }
            }

            // pas dans la liste L1, rechercher dans la liste L2
            if (!etudiantTrouve) {
                for (int i = 0; i < nb_etudiantsL2; i++) {
                    if (strcmp(listeL2[i].matricule, matricule) == 0) {
                        etudiant = listeL2[i];
                        etudiantTrouve = 1;
                        indexEtudiant = i + nb_etudiantsL1;
                        break;
                    }
                }
            }

            if (etudiantTrouve) {
                // Enregistrer la présence de l'étudiant dans le fichier
                time_t now = time(NULL);
                struct tm *tm_now = localtime(&now);
                strftime(heure, sizeof(heure), "%Y-%m-%d %H:%M:%S", tm_now);

                fprintf(fichier, "%s %s %s %d %s\n", etudiant.matricule, etudiant.nom, etudiant.prenom, indexEtudiant, heure);

                // Marquer l'étudiant comme présent pendant cette exécution
                etudiantMarquePresent[indexEtudiant] = 1;

                // Afficher un message indiquant que la présence a été enregistrée avec succès
                printf("La présence de l'étudiant %s %s (%s) a été enregistrée avec succès le %s.\n", etudiant.prenom, etudiant.nom, etudiant.classe, heure);
            } else {
                printf("Matricule invalide. Veuillez réessayer.\n");
            }
        }
        printf("Entrer le matricule de l'étudiant (q pour quitter) : ");
        scanf("%s", matricule);
    }

        while (!motDePasseValide) {
            printf("Entrer le mot de passe administrateur pour revenir au menu : ");
            getHiddenInput(motDePasseAdmin, MAX_BUFFER);

            if (strcmp(motDePasseAdmin, "passer123") == 0 || strcmp(motDePasseAdmin, "passer456") == 0) {
                motDePasseValide = 1;
            } else {
                
            // Permettre à l'utilisateur de saisir à nouveau le matricule de l'étudiant ou q pour quitter
            printf("\nEntrer le matricule de l'étudiant (q pour quitter) : ");
            scanf("%s", matricule);

            if (strcmp(matricule, "q") == 0) {
            break;
            }
            }
    }

    fclose(fichier);
    if (motDePasseValide) {
        afficherMenuAdmin();
    }
}

void marquerPresenceEtudiant(Etudiant listeL1[], int nb_etudiantsL1, Etudiant listeL2[], int nb_etudiantsL2, Utilisateur etudiant) {
    FILE *fichier;
    char modeOuverture[3] = "a";
    FILE *fichierTest = fopen("presence.txt", "r");

    if (fichierTest == NULL) {
        strcpy(modeOuverture, "w");
    }

    fclose(fichierTest);

    fichier = fopen("presence.txt", modeOuverture);

    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier presence.txt.\n");
        return;
    }

    Etudiant etudiantCourant;
    int indexEtudiant = -1;
    int etudiantTrouve = 0;

    // Rechercher l'étudiant dans la liste L1
    for (int i = 0; i < nb_etudiantsL1; i++) {
        if (strcmp(listeL1[i].matricule, etudiant.matricule) == 0) {
            etudiantCourant = listeL1[i];
            etudiantTrouve = 1;
            indexEtudiant = i;
            break;
        }
    }

    if (!etudiantTrouve) {
        for (int i = 0; i < nb_etudiantsL2; i++) {
            if (strcmp(listeL2[i].matricule, etudiant.matricule) == 0) {
                etudiantCourant = listeL2[i];
                etudiantTrouve = 1;
                indexEtudiant = i + nb_etudiantsL1;
                break;
            }
        }
    }

       if (etudiantTrouve) {
        time_t currentTime = time(NULL);
        struct tm* localTime = localtime(&currentTime);
        char dateStr[20];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", localTime);

        int presentAujourdhui = 0;
        char ligne[256];
        FILE *fichierPresence = fopen("presence.txt", "r");
        if (fichierPresence != NULL) {
            while (fgets(ligne, sizeof(ligne), fichierPresence) != NULL) {
                char matricule[MAX_BUFFER], nom[MAX_BUFFER], prenom[MAX_BUFFER], datePresence[20];
                sscanf(ligne, "%s %*s %*s %*s %s", matricule, datePresence);
                if (strcmp(matricule, etudiantCourant.matricule) == 0 && strcmp(datePresence, dateStr) == 0) {
                    presentAujourdhui = 1;
                    break;
                }
            }
            fclose(fichierPresence);
        } else {
            printf("Erreur lors de l'ouverture du fichier presence.txt pour la vérification.\n");
        }

        if (!presentAujourdhui) {
            // Marquer la présence de l'étudiant dans le fichier
            strftime(dateStr, sizeof(dateStr), "%Y-%m-%d %H:%M:%S", localTime);
            fprintf(fichier, "%s %s %s %s %s\n", etudiantCourant.matricule, etudiantCourant.prenom, etudiantCourant.nom, etudiantCourant.classe, dateStr);
            printf("Présence marquée pour l'étudiant %s %s à %s.\n", etudiantCourant.prenom, etudiantCourant.nom, dateStr);
        } else {
            printf("Votre présence a déjà été marquée aujourd'hui. Veuillez choisir une autre option.\n");
        }
    } else {
        printf("Erreur : Impossible de trouver l'étudiant dans la liste.\n");
    }

    fclose(fichier);
}


int validateDate(const char* dateStr) {

     if (dateStr[0] == '\0') {
        // La chaîne est vide, demander à l'utilisateur de ressaisir
        return 0;
    }
    int jour, mois, annee;
    if (sscanf(dateStr, "%d/%d/%d", &jour, &mois, &annee) != 3) {
        return 0; // Format de date invalide
    }

    time_t currentTime = time(NULL);
    struct tm* localTime = localtime(&currentTime);
    int currentYear = localTime->tm_year + 1900;
    int currentMonth = localTime->tm_mon + 1;
    int currentDay = localTime->tm_mday;

    if (annee < 1 || mois < 1 || mois > 12 || jour < 1 || jour > 31) {
        return 0; // Date invalide
    }

    // Vérifier les jours selon le mois
    int maxJours[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (annee % 4 == 0 && (annee % 100 != 0 || annee % 400 == 0)) {
        maxJours[1] = 29; // Année bissextile
    }
    if (jour > maxJours[mois - 1]) {
        return 0; // Jour invalide pour ce mois
    }

    if (annee > currentYear || (annee == currentYear && mois > currentMonth) || (annee == currentYear && mois == currentMonth && jour > currentDay)) {
        return 0; // Date supérieure à la date actuelle
    }

    return 1; // Date valide
}

int fichierExistant(const char *fichiernom) {
    FILE *file;
    if ((file = fopen(fichiernom, "r"))) {
        fclose(file);
        return 1;
    }
    return 0;
}
void genererFichierPresenceParDate() {
    char dateStr[11];
    int dateValide = 0;

    while (!dateValide) {
        printf("Entrer une date au format jj/mm/aaaa : ");
        fgets(dateStr, sizeof(dateStr), stdin);
        dateStr[strcspn(dateStr, "\n")] = '\0';
        dateValide = validateDate(dateStr);

        if (!dateValide) {
            printf("Date invalide. Veuillez ressaisir.\n");
        }
    }

    int jourSaisi, moisSaisi, anneeSaisi;
    sscanf(dateStr, "%d/%d/%d", &jourSaisi, &moisSaisi, &anneeSaisi);

    char nomFichier[30]; // Augmenter la taille pour accueillir un suffixe
    char suffixe[5]; // Pour stocker le suffixe numérique
    int i = 0;

    do {
        if (i > 0) {
            sprintf(suffixe, "_%d", i); // Ajoute un suffixe au nom du fichier
        } else {
            suffixe[0] = '\0'; // Pas de suffixe pour le premier fichier
        }

        sprintf(nomFichier, "%02d_%02d_%04d_presence%s.txt", jourSaisi, moisSaisi, anneeSaisi, suffixe);
        i++; // Incrémente le compteur de suffixe

    } while (fichierExistant(nomFichier)); // Vérifie si le fichier existe déjà

    FILE* fichierPresence = fopen("presence.txt", "r");
    if (fichierPresence == NULL) {
        printf("Erreur lors de l'ouverture du fichier presence.txt.\n");
        return;
    }

    FILE* fichierGenere = fopen(nomFichier, "w");
    if (fichierGenere == NULL) {
        printf("Erreur lors de la création du fichier %s.\n", nomFichier);
        fclose(fichierPresence);
        return;
    }

        fprintf(fichierGenere, "%-12s | %-20s | %-25s | %-10s | %-20s\n", "Matricule", "Prénom", "Nom", "Classe", "Heure marquage présence");
    char ligne[MAX_BUFFER];
    int presencesTrouvees = 0;

      while (fgets(ligne, sizeof(ligne), fichierPresence) != NULL) {
        char matricule[MAX_BUFFER], nom[MAX_BUFFER], prenom[MAX_BUFFER], classe[MAX_BUFFER], datePresence[MAX_BUFFER], heure[MAX_BUFFER];
        int jour, mois, annee;
        sscanf(ligne, "%s %s %s %s %d-%d-%d %s", matricule, nom, prenom, classe, &annee, &mois, &jour, heure);

        if (jour == jourSaisi && mois == moisSaisi && annee == anneeSaisi) {
            fprintf(fichierGenere, "%-12s | %-20s | %-25s | %-10s | %-20s\n", matricule, prenom, nom, classe, heure);
            presencesTrouvees = 1;
        }
    }

    fclose(fichierPresence);
    fclose(fichierGenere);

    if (presencesTrouvees) {
        printf("Le fichier %s a été généré avec succès.\n", nomFichier);
    } else {
        printf("Pas de présence pour cette date.\n");
        remove(nomFichier);
    }
}

