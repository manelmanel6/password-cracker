#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>

#define MAX_USERNAME 50
#define MAX_PASSWORD 50
#define MIN_USERNAME 5
#define MIN_PASSWORD 8
#define SALT_LENGTH 5
#define HASH_LENGTH 65
#define PASSWORD_FILE "password.txt"

typedef struct {
    char username[MAX_USERNAME];
    char password_hash[HASH_LENGTH];
    char salt[SALT_LENGTH + 1];
    char status[20];
} User;

void generate_salt(char *salt) {
    srand(time(NULL));
    for (int i = 0; i < SALT_LENGTH; i++) {
        salt[i] = '0' + (rand() % 10);
    }
    salt[SALT_LENGTH] = '\0';
}

void hash_password(const char *password, const char *salt, char *hash) {
    char combined[MAX_PASSWORD + SALT_LENGTH + 1];
    sprintf(combined, "%s%s", password, salt);
    
    unsigned long h = 5381;
    int c;
    char *str = combined;
    
    while ((c = *str++))
        h = ((h << 5) + h) + c;
    
    sprintf(hash, "%016lx", h);
}

int validate_username(const char *username) {
    int len = strlen(username);

    if (len < 5) {
        printf("Erreur: Le username doit contenir au moins 5 caracteres\n");
        return 0;
    }

    for (int i = 0; i < len; i++) {
        if (!isalnum(username[i])) {
            printf("Erreur: Le username ne doit contenir que lettres et chiffres\n");
            return 0;
        }
    }

    return 1;
}


int validate_password(const char *password) {
    int len = strlen(password);

    if (len < 8) {
        printf("Erreur: Le password doit contenir au moins 8 caracteres\n");
        return 0;
    }

    for (int i = 0; i < len; i++) {
        if (isalnum(password[i])) {
            continue;
        }
        else if (password[i] == '@' || password[i] == '#' ||
                 password[i] == '$' || password[i] == '&') {
            continue;
        }
        else {
            printf("Erreur: Caractere non autorise dans le password\n");
            return 0;
        }
    }

    return 1;
}


int load_user(const char *username, User *user) {
    FILE *file = fopen(PASSWORD_FILE, "r");
    if (file == NULL) {
        return 0;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        User temp;
        if (sscanf(line, "%[^:]:%[^:]:%[^:]:%s", 
                   temp.username, temp.password_hash, temp.salt, temp.status) == 4) {
            if (strcmp(temp.username, username) == 0) {
                *user = temp;
                fclose(file);
                return 1;
            }
        }
    }
    
    fclose(file);
    return 0;
}

void save_user(const User *user) {
    FILE *file = fopen(PASSWORD_FILE, "a");
    if (file == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        return;
    }
    
    fprintf(file, "%s:%s:%s:%s\n", 
            user->username, user->password_hash, user->salt, user->status);
    
    fclose(file);
}

void signup() {
    system("cls");
    printf("\n============================================\n");
    printf("           INSCRIPTION\n");
    printf("============================================\n\n");
    
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    
    while (1) {
        printf("Username (min 5 caracteres: min, MAJ, 0-9): ");
        scanf("%s", username);
        fflush(stdin);
        
        if (!validate_username(username)) {
            continue;
        }
        
        User existing;
        if (load_user(username, &existing)) {
            printf("Erreur: Ce username existe deja\n");
            continue;
        }
        
        break;
    }
    
    while (1) {
        printf("Password (min 8 caracteres: min, MAJ, 0-9, @#$&): ");
        scanf("%s", password);
        fflush(stdin);
        
        if (validate_password(password)) {
            break;
        }
    }
    
    User new_user;
    strcpy(new_user.username, username);
    generate_salt(new_user.salt);
    hash_password(password, new_user.salt, new_user.password_hash);
    strcpy(new_user.status, "active");
    
    save_user(&new_user);
    
    printf("\nCompte cree avec succes pour '%s'!\n", username);
    printf("\nAppuyez sur Entree pour continuer...");
    fflush(stdin);
    getchar();
}

// post authentication messages
int post_auth_menu() {
    int choice;
    printf("\n============================================\n");
    printf("  1. Retour au menu principal\n");
    printf("  2. Quitter le programme\n");
    printf("============================================\n");
    printf("Votre choix (1-2): ");

    if (scanf("%d", &choice) != 1) {
        fflush(stdin);
        return 1;
    }
    fflush(stdin);

    if (choice == 2) {
        printf("\nAu revoir!\n");
        exit(0);
    }

    return 1; // retour au menu
}



//update account statue in password.txt 
void update_user_status(const char *username, const char *new_status) {
    FILE *file = fopen(PASSWORD_FILE, "r");
    FILE *temp = fopen("temp.txt", "w");

    if (!file || !temp) return;

    char line[256];
    User u;

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%[^:]:%[^:]:%[^:]:%s",
                   u.username, u.password_hash, u.salt, u.status) == 4) {

            if (strcmp(u.username, username) == 0) {
                strcpy(u.status, new_status);
            }

            fprintf(temp, "%s:%s:%s:%s\n",
                    u.username, u.password_hash, u.salt, u.status);
        }
    }

    fclose(file);
    fclose(temp);

    remove(PASSWORD_FILE);
    rename("temp.txt", PASSWORD_FILE);
}


void signin() {
    system("cls");
    printf("\n============================================\n");
    printf("           CONNEXION\n");
    printf("============================================\n\n");
    
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];
    
    printf("Username: ");
    scanf("%s", username);
    fflush(stdin);
    
    User user;
    if (!load_user(username, &user)) {
        printf("\nUsername incorrect\n");
        printf("\nAppuyez sur Entree pour continuer...");
        fflush(stdin);
        getchar();
        return;
    }
    
    if (strcmp(user.status, "banned") == 0) {
        printf("\n============================================\n");
        printf(" CE COMPTE EST BANNI\n");
        printf(" Acces refuse\n");
        printf("============================================\n");

        post_auth_menu();
        return;

    }
    
    int block_times[] = {5, 10, 15, 20};
    int round = 0;
    
    while (round <= 4) {
        for (int attempt = 0; attempt < 3; attempt++) {
            printf("\nPassword (tentative %d/3): ", attempt + 1);
            scanf("%s", password);
            fflush(stdin);
            
            char input_hash[HASH_LENGTH];
            hash_password(password, user.salt, input_hash);
            
                if (strcmp(input_hash, user.password_hash) == 0) {
                    printf("\n============================================\n");
                    printf(" Connexion reussie!\n");
                    printf(" Bienvenue %s\n", username);
                    printf("============================================\n");

                    post_auth_menu();
                    return;

            } else {
                printf("Password incorrect\n");
            }
        }
        
        if (round < 4) {
            int block_time = block_times[round];
            printf("\nTrop de tentatives. Blocage pendant %d secondes...\n", block_time);
            Sleep(block_time * 1000);
            printf("Vous pouvez reessayer.\n");
            round++;
        } else {
            printf("\n============================================\n");
            printf(" COMPTE BANNI DEFINITIVEMENT!\n");
            printf(" Trop de tentatives de connexion\n");
            printf("============================================\n");

            update_user_status(username, "banned");

            post_auth_menu();
            return;

        }

    }
}

int main() {
    int choice;
    
    while (1) {
        system("cls");
        printf("\n============================================\n");
        printf("      SYSTEME D'AUTHENTIFICATION\n");
        printf("============================================\n\n");
        printf("  1. Inscription (Signup)\n");
        printf("  2. Connexion (Signin)\n");
        printf("  3. Quitter (Exit)\n\n");
        printf("============================================\n");
        printf("\nVotre choix (1-3): ");
        
        if (scanf("%d", &choice) != 1) {
            fflush(stdin);
            printf("\nChoix invalide\n");
            Sleep(1000);
            continue;
        }
        fflush(stdin);
        
        switch (choice) {
            case 1:
                signup();
                break;
            case 2:
                signin();
                break;
            case 3:
                printf("\nAu revoir!\n");
                return 0;
            default:
                printf("\nChoix invalide\n");
                Sleep(1000);
        }
    }
    
    return 0;
}