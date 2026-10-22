#include <stdio.h>
#include <string.h>

#define PASS_LEN 8

char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#$&";

/* === DJB2 HASH  === */
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
    char target_user[50];
    char stored_hash[65], salt[10], status[20];
    char line[256];

    printf("Username a cracker: ");
    scanf("%s", target_user);

    FILE *f = fopen("password.txt", "r");
    if (!f) {
        printf("Erreur ouverture fichier\n");
        return 1;
    }

    int found = 0;
    while (fgets(line, sizeof(line), f)) {
        char u[50];
        sscanf(line, "%[^:]:%[^:]:%[^:]:%s", u, stored_hash, salt, status);
        if (strcmp(u, target_user) == 0) {
            found = 1;
            break;
        }
    }
    fclose(f);

    if (!found) {
        printf("Username non trouve\n");
        return 1;
    }

    char pass[PASS_LEN + 1];
    char hash[65];
    int n = strlen(charset);

    printf("Crack en cours...\n");

    for (int a=0;a<n;a++)
    for (int b=0;b<n;b++)
    for (int c=0;c<n;c++)
    for (int d=0;d<n;d++)
    for (int e=0;e<n;e++)
    for (int f1=0;f1<n;f1++)
    for (int g=0;g<n;g++)
    for (int h=0;h<n;h++) {

        pass[0]=charset[a];
        pass[1]=charset[b];
        pass[2]=charset[c];
        pass[3]=charset[d];
        pass[4]=charset[e];
        pass[5]=charset[f1];
        pass[6]=charset[g];
        pass[7]=charset[h];
        pass[8]='\0';

        hash_password(pass, salt, hash);

        if (strcmp(hash, stored_hash) == 0) {
            printf("\nPASSWORD TROUVE: %s\n", pass);
            return 0;
        }
    }

    printf("Password non trouve\n");
    return 0;
}
