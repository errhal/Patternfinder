#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *kmp_search(char *, char *, int *);

int main() {

    char *option = malloc(1024);
    char *filename = malloc(1024);

    printf("Load text file: \n");
    scanf("%s", filename);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("File doesn't exist !");
        return -1;
    }
    char *buffer = malloc(1024);
    fgets(buffer, 1024, file);

    printf("Select option you want to use: \n1. search - find pattern in text from file\n2. exit - quit program\n");
    while (1) {
        scanf("%s", option);

        if (strcmp(option, "search") == 0) {
            printf("Type word you want search:\n");
            char *pattern = malloc(1024);
            scanf("%s", pattern);

            int positions_count = 0;
            int *positions = kmp_search(buffer, pattern, &positions_count);
            printf("Pattern found on positions: ");
            for (int i = 0; i < positions_count; i++) {
                printf("%d ", positions[i]);
            }

        } else if (strcmp(option, "exit") == 0) break;
    }


}

int *kmp_search(char *buffer, char *pattern, int *positions_count) {

    int *T = malloc(sizeof(*T) * 512);
    int m = 0, i = 2, j = 0;
    T[0] = -1;
    T[1] = 0;
    while (i < strlen(pattern)) {
        if (pattern[i - 1] == pattern[j]) {
            T[i] = j + 1;
            i++;
            j++;
        } else if (j > 0) {
            j = T[j];
        } else {
            T[i] = 0;
            i++;
        }
    }

    m = i = 0;
    int *positions = malloc(1024);
    *positions_count = 0;

    while (m + i < strlen(buffer)) {
        if (pattern[i] == buffer[i + m]) {
            if (i == strlen(pattern) - 1) {
                positions[*positions_count] = m;
                (*positions_count)++;
            }
            i++;
        } else {
            if (T[i] > -1) {
                m = m + i - T[i];
                i = T[i];
            } else {
                m++;
                i = 0;
            }
        }

    }

    return positions;
}
