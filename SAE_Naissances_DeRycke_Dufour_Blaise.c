#include <stdio.h> //Permet de lire depuis la console et d'écrire vers la console 
#include <stdlib.h> //Permet d'utiliser diverses fonctions liées à la gestion de la mémoire
#include <string.h> //Permet la manipulation de chaînes de caractères
#include <time.h> //Permet l'utilisation des dates
#include <wchar.h> //Permet l'utilisation des caractères larges
#include <locale.h> //Permet l'utilisation des caractères spéciaux
#define Longueur_max_nom 25
#define Taille_chunk 8192

// Structure pour stocker les données extraites du fichier CSV
struct EntreNom {
    char genre;
    wchar_t nom[Longueur_max_nom];
    int annee;
    int count;
};

// Affichage du menu
void afficherMenu() { // Affichage du menu\n");
    printf("\nMerci de choisir un item :\n\n");
    printf("0. Ce menu :\n");
    printf("1. Le nombre de naissances\n");
    printf("2. Le nombre de prenoms\n");
    printf("3. Statistiques sur un prenom \n");
    printf("4. Quitter\n");
    
}

// Fonction de comparaison pour le tri des structures
int compareCounts(const void *a, const void *b) {
    return ((struct EntreNom *)b)->count - ((struct EntreNom *)a)->count;
}

//Stocke les données filtrées sur le genre et le nom
typedef struct {
    int genre;
    char Nom[Longueur_max_nom];
} Person;


// Premier item - Nombre de naissances
void AdditionNaissance(const char *nomfichier) {
    FILE *file = fopen(nomfichier, "r");

    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(1);
    }

    char ligne[1024];
    int sum = 0; // Variable pour stocker la somme des valeurs de la cinquième colonne

    // Lis le fichier CSV et additionne les valeurs de la cinquième colonne
    while (fgets(ligne, sizeof(ligne), file) != NULL) {
        char *val = strtok(ligne, ";");
        
        for (int i = 0; i < 4; ++i) {
            // Ignore les 4 premières colonnes
            val = strtok(NULL, ";");
        }

        if (val != NULL) {
            // Récupération des valeurs et addition de celles-ci
            sum += atoi(val);
        }
    }

    // Affiche le résultat
    wprintf(L"\nLe nombre total de naissances en France de 1900 à 2022 est de : %d\n", sum);

    fclose(file);
}


// Deuxième item - Nombre de prénoms
void NombrePrenom(const char* chemin_fichier, int diff_genre) {
    FILE* file = fopen(chemin_fichier, "r");

    if (!file) {
        perror("Erreur a l'ouverture du fichier");
        exit(EXIT_FAILURE);
    }

    // Gère le header du fichier
    char header[Longueur_max_nom * 2];
    if (fgets(header, sizeof(header), file) == NULL) {
        perror("Error reading header");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Gère le buffer pour en compter les entrées
    int count = 0;
    int nom_fem = 0;
    int nom_masc = 0;

    char buffer[Taille_chunk];
    char* line;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        line = strchr(buffer, '\n');

        // Gère les lignes du buffer
        while (line != NULL) {
            *line = '\0';  // Termine la ligne

            int genre;
            char Nom[Longueur_max_nom];

            // Utilise sscanf pour extraire le genre et le nom de la ligne
            if (sscanf(buffer, "%d;%255[^\n]", &genre, Nom) == 2) {
                count++;

                // Compte les noms en fonction des genres
                if (diff_genre) {
                    if (genre == 2) {
                        nom_fem++;
                    } else if (genre == 1) {
                        nom_masc++;
                    }
                }
            } else {
                fprintf(stderr, "Erreur de lecture: %s\n", buffer);
            }

            // Passe à la ligne suivante
            buffer[0] = '\0';
            line = strchr(line + 1, '\n');
        }
    }

    fclose(file);

    if (diff_genre) {
        // Affiche le nombre de noms féminins et masculins
        wprintf(L"\nIl y a %d noms féminins et %d noms masculins.\n", nom_fem, nom_masc);
    } else {
        // Affiche le nombre total de noms différents
        wprintf(L"\nIl y a %d noms différents.\n", count);
    }
}


// Troisième item - Choix du prénom
void ChoixPrenom(const char *nomfichier, const wchar_t *NomSelect, char GenreSelect) {
    struct EntreNom Entree;
    FILE *file = fopen(nomfichier, "r");

    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        exit(1);
    }

    char ligne[1024];  // Lignes de 1024 caractère maximum
    int EntreeCorresp = 0; // Compteur pour les entrées correspondantes au prénom et genre sélectionnés
    int PremiereAnnee = -1; // Année d'apparition la plus ancienne
    int DerniereAnnee = -1; // Année d'apparition la plus récente
    // Total d'occurrences pour les 'valeurs' de genres
    int totalOccurrences0 = 0;
    int totalOccurrences1 = 0;
    int totalOccurrences2 = 0;

    wprintf(L"Résultats pour le prénom '%ls' et le genre'%c' :\n", NomSelect, GenreSelect);

    // Recherche le prénom demandé dans le fichie CSV
    while (fgets(ligne, sizeof(ligne), file) != NULL) {
        char *val = strtok(ligne, ";");

        if (val != NULL) {
            Entree.genre = val[0];

            // Récupère la colonne "prénom"
            val = strtok(NULL, ";");
            mbstowcs(Entree.nom, val, Longueur_max_nom - 1);
            Entree.nom[Longueur_max_nom - 1] = L'\0';  // Informe de la fin de la ligne par un caractère NULL

            // Récupère la colonne "année"
            val = strtok(NULL, ";");

            // Ignore les lignes où "année" est soit à "0" ou à "XXXX"
            if (strcmp(val, "0") == 0 || strcmp(val, "XXXX") == 0) {
                continue;
            }

            Entree.annee = atoi(val);

            // Récupère la colonne nombre
            val = strtok(NULL, ";");
            Entree.count = atoi(val);

            // Vérifie que les données correspondent au genre et sexe selectionné
            if ((wcscmp(Entree.nom, NomSelect) == 0) && 
                (GenreSelect == '0' || Entree.genre == GenreSelect)) {
                EntreeCorresp++;

                // Première année et dernière année d'apparition
                if (PremiereAnnee == -1 || Entree.annee < PremiereAnnee) {
                    PremiereAnnee = Entree.annee;
                }
                if (DerniereAnnee == -1 || Entree.annee > DerniereAnnee) {
                    DerniereAnnee = Entree.annee;
                }

                // Total d'occurences pour chaque "valeurs" de genre
                if (Entree.genre == '0') {
                    totalOccurrences0 += Entree.count;
                } else if (Entree.genre == '1') {
                    totalOccurrences1 += Entree.count;
                } else if (Entree.genre == '2') {
                    totalOccurrences2 += Entree.count;
                }
            }
        }
    }

    if (EntreeCorresp > 0) {
        wprintf(L"\nPremière année d'apparition : %d\n", PremiereAnnee);
        wprintf(L"Dernière année d'apparition : %d\n", DerniereAnnee);

        // Affiche les occurences pour masculin et féminin, ainsi que le total
        if (GenreSelect == '0') {
            wprintf(L"Occurrences pour genre masculin : %d\n", totalOccurrences1);
            wprintf(L"Occurrences pour genre féminin : %d\n", totalOccurrences2);
            wprintf(L"Total d'occurrences : %d\n", totalOccurrences1 + totalOccurrences2);
        } else {
            // Affiche le total d'occurences
            wprintf(L"Occurrences pour le genre '%c' : %d\n", GenreSelect, 
                (GenreSelect == '1') ? totalOccurrences1 : totalOccurrences2);
        }
    } else {
        // Affiche un message si aucune entrée ne correspond au prénom et genre sélectionnés
        wprintf(L"Aucun résultat pour le prénom '%ls' et le genre '%c'.\n", NomSelect, GenreSelect);
    }

    fclose(file);
}

// Fonction principale
int main() {
    int choix;
    const char nomFichier[] = "dpt2022.csv";
    setlocale(LC_ALL, ""); // Permet l'affichage des caractères spéciaux

    wchar_t NomSelect[Longueur_max_nom]; // Variable pour stocker le prénom choisi
    char GenreSelect; // Variable pour stocker le genre choisi
    const char* chemin_fichier = "dpt2022.csv";
    int diff_genre;
    
    // Demande à l'utilisateur ce qu'il veut afficher
    printf("Que voulez-vous afficher ? (0 pour le menu)\n ");
    scanf("%d", &choix); // Demande l'entrée du choix de l'utilisateur
    if (choix == 0){
        do {
            afficherMenu(); // Affiche le menu
            printf("Que voulez-vous faire ? : ");
            scanf("%d", &choix);
            switch (choix) {
                
                case 1:
                    AdditionNaissance("dpt2022.csv");           
                    break;

                case 2:
                    //Demande à l'utilisateur s'il veut distinguer les genres dans le comptage des prénoms
                    printf("Voulez-vous distinguer les genres ? (1. OUI, 0. NON): ");
                    scanf("%d", &diff_genre);

                    //Résultat
                    NombrePrenom(chemin_fichier, diff_genre);
                    break;

                case 3:
                    // Demande à l'utilisateur.ice le prénom recherché
                    wprintf(L"Entrez le prénom à rechercher (tout en majuscules) : \n");
                    wscanf(L"%ls", NomSelect);

                    // Demande à l'utilisateur.ice s'il faut filtrer par genre
                    wprintf(L"Voulez-vous filtrer par genre ? Entrez '1' pour 'masculin', '2' pour 'féminin', ou '0' pour 'tout' : \n");
                    scanf(" %c", &GenreSelect);

                    // Affiche les statistiques pour le prénom spécifié
                    ChoixPrenom("dpt2022.csv", NomSelect, GenreSelect);
                    break;

                case 4:
                    printf("\nVous venez de quitter le programme\n\n");
                    
                    break;// Arrête le programme

                default:
                    // Affiche un message d'erreur pour un choix invalide
                    printf("Choix invalide.\n");
            }
        }while (choix != 4); // Continue la boucle jusqu'à selection de 4
    }
    else{ //Arrête le programme si 0 non choisit
        printf("Veuillez entrez 0 si vous souhaitez avoir le menu");
        exit(1);
    }
    return 0; 
} 
