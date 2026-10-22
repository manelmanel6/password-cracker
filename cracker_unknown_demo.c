#include <stdio.h>
#include <string.h>

#define MAX_PASSWORD 50
#define HASH_LENGTH 65
#define SALT_LENGTH 5

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

int main() {
    FILE *users = fopen("password.txt", "r");
    FILE *dict = fopen("dictionary.txt", "r");

    if (!users || !dict) {
        printf("Fichiers manquants\n");
        return 0;
    }

    User user;
    char guess[MAX_PASSWORD];
    char hash[HASH_LENGTH];

    while (fscanf(users, "%[^:]:%[^:]:%[^:]:%s\n",
                  user.username,
                  user.password_hash,
                  user.salt,
                  user.status) == 4) {

        rewind(dict);

        while (fscanf(dict, "%s", guess) == 1) {
            hash_password(guess, user.salt, hash);

            if (strcmp(hash, user.password_hash) == 0) {
                printf("\n[COMPTE CRAQUE]\n");
                printf("Username: %s\n", user.username);
                printf("Password: %s\n", guess);
                break;
            }
        }
    }

    fclose(users);
    fclose(dict);
    return 0;
}
