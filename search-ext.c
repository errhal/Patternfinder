#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *kmp_search(char *, char *, int *, int, int);

int read_buffer(char *, FILE *, int);

int main() {

    int positions_count = 0, max_positions_count = 1024, *positions, partial_positions_count, *tmp_positions;
    int i, pattern_size = 0, max_pattern_size = 8, current_offset = 0, max_frame;
    char *option = malloc(1024);
    char *filename = malloc(1024);
    FILE *file;
    char *buffer;    // current loaded text from file
    char *pattern = malloc((max_pattern_size + 1) * sizeof(char)), c;

    printf("Load text file: \n");
    scanf("%s", filename);
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("File doesn't exist !");
        return -1;
    }

    printf("Select option you want to use: \n1. search - find pattern in text from file\n2. exit - quit program\n");
    while (1) {
        scanf("%s", option);
        if (strcmp(option, "search") == 0 || strcmp(option, "1") == 0) {
            printf("Type word you want search:\n");

            while ((c = (char) getchar()) != '\n' || pattern_size == 0) {
                if (c == '\n') continue;
                if (pattern_size == max_pattern_size) {
                    pattern = realloc(pattern, ((2 * max_pattern_size) + 1) * sizeof(char));
                    max_pattern_size *= 2;
                }
                pattern[pattern_size++] = c;
            }
            pattern[pattern_size] = 0x00;

            max_frame = pattern_size * 1024 < 1000000 ? 1000000 : pattern_size * 1024;
            while ((buffer = malloc(max_frame * sizeof(char))) == NULL) {
                max_frame /= 2;
            }
            if (buffer == NULL || max_frame <= pattern_size) {
                printf("Cannot allocate enough memory for this pattern!\n");
                continue;
            }

            positions_count = 0;
            positions = malloc(max_positions_count * sizeof(*positions));
            fseek(file, 0, SEEK_SET);

            read_buffer(buffer, file, max_frame);
            do {
                tmp_positions = kmp_search(buffer, pattern, &partial_positions_count, max_frame, current_offset);
                while (positions_count + partial_positions_count > max_positions_count) {
                    max_positions_count *= 2;
                    positions = realloc(positions, max_positions_count * sizeof(*positions));
                    if(positions == NULL) {
                        printf("It's impossible to allocate more memory for positions array!");
                        return -1;
                    }
                }
                memcpy(&positions[positions_count], tmp_positions, partial_positions_count * sizeof(*positions));
                positions_count += partial_positions_count;
                free(tmp_positions);

                current_offset += max_frame - pattern_size;
                fseek(file, current_offset, SEEK_SET); // inefficient SEEK
            } while (read_buffer(buffer, file, max_frame) != NULL);

            printf("Number of patterns found: %d", positions_count);
            if (positions_count > 0) {
                printf("\nPattern found on positions: ");
                for (i = 0; i < positions_count; i++) {
                    printf("%d ", positions[i]);
                }
            }
            printf("\n\n");
            puts("Select option you want to use: \n1. search - find pattern in text from file\n2. exit - quit program");
            fflush(stdout);
            pattern_size = 0;
            current_offset = 0;
            free(positions);
            free(buffer);
        } else if (strcmp(option, "exit") == 0 || strcmp(option, "2") == 0) {
            free(option);
            free(filename);
            free(pattern);
            break;
        } else {
            puts("You typed bad parameter!");
        }
    }
}

int *kmp_search(char *buffer, char *pattern, int *positions_count, int max_frame, int file_offset) {

    int *T = malloc(sizeof(*T) * strlen(buffer)), *positions;
    int m = 0, i = 1, j = 0;
    T[0] = -1;
    while (i < strlen(pattern)) {
        if (pattern[i] == pattern[j]) {
            T[i] = T[j];
            i++;
            j++;
        } else {
            T[i] = j;
            j = T[j];
            while (j >= 0 && pattern[i] != pattern[j]) {
                j = T[j];
            }
            i++;
            j++;
        }
    }
    T[i] = j;

    m = i = 0;
    positions = malloc(strlen(buffer) * sizeof(*positions));
    *positions_count = 0;

    while (m + i < strlen(buffer)) {
        if (pattern[i] == buffer[i + m]) {
            if (i == strlen(pattern) - 1) {
                positions[*positions_count] = m + file_offset;
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
    free(T);
    return positions;
}

int read_buffer(char *buffer, FILE *file, int length) {
    int i = -1;
    char c;

    while ((++i < length - 1)) {
        c = (char) fgetc(file);
        if (c == EOF) {
            buffer[i] = 0x00;
            if (i == 0) return NULL;
            return 0;
        }
        buffer[i] = c;
    }
    buffer[i] = 0x00;
}
