#include <stdio.h>
#include <string.h>

#define MAX_PASSWORD 50
#define HASH_LENGTH 65
#define SALT_LENGTH 5
#define PASSWORD_FILE "password.txt"
#define DICT_FILE "dictionary.txt"

typedef struct {
    char username[50];
    char password_hash[HASH_LENGTH];
    char salt[SALT_LENGTH + 1];
    char status[20];
} User;

void hash_password(const char *password, const char *salt, char *hash) {
    char combined[100];
    sprintf(combined, "%s%s", password, salt);

    unsigned long h = 5381;
    int c;
    char *str = combined;

    while ((c = *str++))
        h = ((h << 5) + h) + c;

    sprintf(hash, "%016lx", h);
}

int load_user(const char *username, User *user) {
    FILE *f = fopen(PASSWORD_FILE, "r");
    if (!f) return 0;

    while (fscanf(f, "%[^:]:%[^:]:%[^:]:%s\n",
                  user->username,
                  user->password_hash,
                  user->salt,
                  user->status) == 4) {
        if (strcmp(user->username, username) == 0) {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int main() {
    char username[50];
    char guess[MAX_PASSWORD];
    char hash[HASH_LENGTH];
    User user;

    printf("Username cible: ");
    scanf("%s", username);

    if (!load_user(username, &user)) {
        printf("Utilisateur introuvable\n");
        return 0;
    }

    FILE *dict = fopen(DICT_FILE, "r");
    if (!dict) {
        printf("Dictionnaire introuvable\n");
        return 0;
    }

    while (fscanf(dict, "%s", guess) == 1) {
        hash_password(guess, user.salt, hash);

        if (strcmp(hash, user.password_hash) == 0) {
           
            printf("PASSWORD TROUVE : %s\n", guess);
            fclose(dict);
            return 0;
        }
    }

    printf("\nMot de passe non trouve dans le dictionnaire\n");
    fclose(dict);
    return 0;
}
