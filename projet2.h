#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h> // ajouté pour utiliser time_t, struct tm, time, et strftime
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
// Prototypes des fonctions
char getch();
int verifierIdentifiants(const char *matricule, const char *motdepasse, int *typeUtilisateur, int *etatUtilisateur);
void getHiddenInput(char *password, int maxLength);
Utilisateur interfaceConnexion();
void creerFichierAdminEtudiant();
void afficherMenuAdmin();
void afficherMenuEtudiant();
void initialiserEtudiantsL1(Etudiant liste[], int *nb_etudiants);
void initialiserEtudiantsL2(Etudiant liste[], int *nb_etudiants);
void marquerPresence();

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
            if (typeUtilisateur == 1 && etatUtilisateur == 1)
                afficherMenuAdmin();
            else if (typeUtilisateur == 2 && etatUtilisateur == 1)
                afficherMenuEtudiant();
            break; 
        } else {
            printf("Échec de la connexion. Identifiants incorrects.\n");
        }
    }
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

void afficherMenuAdmin() {
    int choix;
    printf("\n");
    printf("***********Bienvenue dans votre Espace ADMIN.***********\n");
    printf("\n");
    do{
    printf("\n1. Gestion des etudiants\n");
    printf("2. Generation de fichier\n");
    printf("3. Marquer les presences\n");
    printf("4. Envoyer un message\n");
    printf("5. Parametres\n");
    printf("6. Quitter\n");
    scanf("%d", &choix);
    if (choix < 1 || choix > 6) {
            printf("Choix invalide\n");
            scanf("%d",&choix);
        }
    switch(choix) {
        case 3:
            marquerPresence();
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
    }while(choix != 6);
}

void afficherMenuEtudiant() {
    int ch;
    printf("\n");
    printf("***********Bienvenue dans votre Espace ETUDIANT.***********\n");
    printf("\n");
    do{
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
    }while(ch!=4);
}

void initialiserEtudiantsL1(Etudiant liste[], int *nb_etudiants) {
    FILE *fichier = fopen("listeL1.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier listeL1.txt.\n");
        exit(1);
    }

    *nb_etudiants = 0;
    while (fscanf(fichier, "%s %s %s %s %s %s %d %d %d", liste[*nb_etudiants].matricule, liste[*nb_etudiants].prenom, liste[*nb_etudiants].nom, liste[*nb_etudiants].dateNaissance, liste[*nb_etudiants].motDePasse, liste[*nb_etudiants].classe, &liste[*nb_etudiants].nbrAbsence, &liste[*nb_etudiants].cumulRetard, &liste[*nb_etudiants].presence) == 9) {
        (*nb_etudiants)++;
    }

    fclose(fichier);
}

void initialiserEtudiantsL2(Etudiant liste[], int *nb_etudiants) {
    FILE *fichier = fopen("listeL2.txt", "r");
    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier listeL2.txt.\n");
        exit(1);
    }

    *nb_etudiants = 0;
    while (fscanf(fichier, "%s %s %s %s %s %s %d %d %d", liste[*nb_etudiants].matricule, liste[*nb_etudiants].prenom, liste[*nb_etudiants].nom, liste[*nb_etudiants].dateNaissance, liste[*nb_etudiants].motDePasse, liste[*nb_etudiants].classe, &liste[*nb_etudiants].nbrAbsence, &liste[*nb_etudiants].cumulRetard, &liste[*nb_etudiants].presence) == 9) {
        (*nb_etudiants)++;
    }

    fclose(fichier);
}

void marquerPresence() {
    FILE *presenceFile = fopen("presence.txt", "a");
    if (presenceFile == NULL) {
        printf("Erreur lors de l'ouverture du fichier presence.txt.\n");
        return;
    }
    
    printf("\nMarquer la présence d'un étudiant :\n");
    printf("Entrer le matricule de l'étudiant (q pour quitter) : ");
    char matricule[MAX_BUFFER];
    scanf("%s", matricule);
    if (strcmp(matricule, "q") == 0) {
        fclose(presenceFile);
        return;
    }

    // Recherche de l'étudiant dans la liste L1
    for (int i = 0; i < nb_etudiants_L1; i++) {
        if (strcmp(listeL1[i].matricule, matricule) == 0) {
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            char timestamp[MAX_BUFFER];
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm);
            
            fprintf(presenceFile, "%s %s %s %s\n", listeL1[i].prenom, listeL1[i].nom, matricule, timestamp);
            printf("Votre présence a été enregistrée : %s %s %s\n", listeL1[i].prenom, listeL1[i].nom, timestamp);
            fclose(presenceFile);
            return; // Sortir de la fonction après avoir trouvé l'étudiant dans la liste L1
        }
    }

    // Si l'étudiant n'est pas trouvé dans la liste L1, rechercher dans la liste L2
    for (int i = 0; i < nb_etudiants_L2; i++) {
        if (strcmp(listeL2[i].matricule, matricule) == 0) {
            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            char timestamp[MAX_BUFFER];
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm);
            
            fprintf(presenceFile, "%s %s %s %s\n", listeL2[i].prenom, listeL2[i].nom, matricule, timestamp);
            printf("Votre présence a été enregistrée : %s %s %s\n", listeL2[i].prenom, listeL2[i].nom, timestamp);
            break;
        }
    }

    fclose(presenceFile);
}
