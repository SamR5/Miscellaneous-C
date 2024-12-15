#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *path;

int count_lines(char *path);
int cmp_lines(const void *a, const void *b);
void write_sorted(char *path, size_t size, int *array, size_t arrLen);

int count_lines(char *path) {
    FILE *fptr;
    fptr = fopen(path, "r");
    if (fptr == NULL)
        exit(EXIT_FAILURE);
    char *line = NULL;
    size_t len = 0;
    int counter = 0;
    while(getline(&line, &len, fptr) != -1)
        counter++;
    free(line);
    if (fclose(fptr) != 0)
        exit(EXIT_FAILURE);
    return counter;
}

int cmp_lines(const void *a, const void *b) {
    int *A = (int *)a;
    int *B = (int *)b;
    if (A == B)
        return 0;
    FILE *fptr;
    fptr = fopen(path, "r");
    if (fptr == NULL)
        exit(EXIT_FAILURE);
    char *line = NULL;
    char lineA[128];
    char lineB[128];
    size_t len = 0;
    int counter = 0;
    while(getline(&line, &len, fptr) != -1) {
        if (counter > *A && counter > *B)
            break;
        if (counter == *A) {
            strcpy(lineA, line);//, strlen(line));
        } else if (counter == *B) {
            strcpy(lineB, line);//, strlen(line));
        }
        counter++;
    }
    free(line);
    if (fclose(fptr) != 0)
        exit(EXIT_FAILURE);
    return strncmp(lineA, lineB, 128);
}

void write_sorted(char *path, size_t size, int *array, size_t arrLen) {
    char *newPath = calloc(8+size, sizeof(char));
    strncpy(newPath, "sorted_", 8);
    strncat(newPath, path, size);
    printf("New name: %s\n", newPath);

    FILE *unsortedF, *sortedF;
    unsortedF = fopen(path, "r");
    sortedF = fopen(newPath, "w");

    char* line = NULL;
    size_t len = 0;
    for (size_t i=0; i<arrLen; i++) {
        int j = -1;
        do {
            getline(&line, &len, unsortedF);
            j++;
        } while (j!=array[i]);
        fputs(line, sortedF);
        rewind(unsortedF);
    }
    if (fclose(unsortedF) != 0)
        exit(EXIT_FAILURE);
    if (fclose(sortedF) != 0)
        exit(EXIT_FAILURE);
    free(newPath);
}



int main(int argc, char *argv[]) {
    size_t pathLen = strlen(argv[1]);
    path = calloc(pathLen, sizeof(char));
    strncpy(path, argv[1], pathLen);
    int lines = count_lines(path);
    printf("File %s contains: %d lines\n", path, lines);
    int *array = malloc(lines * sizeof(int));
    for (int i=0; i<lines; i++) {
        array[i] = i;
    }
    printf("Sorting array according to file\n");
    qsort(array, lines, sizeof(int), cmp_lines);
    printf("Write sorted file according to array\n");
    write_sorted(path, pathLen, array, lines);
    free(path);
    free(array);
    return 0;
}
