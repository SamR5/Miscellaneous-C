#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>


char *path;

int count_lines(char *path);
void sort_file(char *path, size_t size, size_t arrLen);

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

void sort_file(char *path, size_t size, size_t lines) {
    char *newPath = calloc(8+size, sizeof(char));
    strncpy(newPath, "sorted_", 8);
    strncat(newPath, path, size);

    FILE *unsortedF, *sortedF;
    unsortedF = fopen(path, "r");
    sortedF = fopen(newPath, "w");

    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    gettimeofday(&tv2, NULL);

    size_t linesSorted = 0;
    bool *toSkip = calloc(lines, sizeof(int));
    printf("Number of lines: %ld\n", lines);
    printf("%ld/%ld\n", linesSorted, lines);
    while (linesSorted < lines) {
        char* line = NULL;
        size_t len = 0;
        char currMin[31] = {255};
        int currMinIndex = 0;
        int counter = 0;
        while(getline(&line, &len, unsortedF) != EOF) {
            counter++;
            if (toSkip[counter])
                continue;
            if (strcmp(currMin, line) > 0) {
                strcpy(currMin, line); // '\0' automatically appended
                currMinIndex = counter;
            }
        }
        fputs(currMin, sortedF);
        rewind(unsortedF);
        toSkip[currMinIndex] = true;
        linesSorted++;
        gettimeofday(&tv2, NULL);
        if (tv2.tv_sec*10 - tv1.tv_sec*10 > 5) {
            gettimeofday(&tv1, NULL);
            //system("clear");
            printf("\33[A\r");
            printf("\33[2K\r");
            printf("%ld/%ld\n", linesSorted, lines);
        }
    }
    printf("\33[A\r");
    printf("\33[2K\r");
    printf("%ld/%ld\n", linesSorted, lines);
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
    sort_file(path, pathLen, lines);
    free(path);
    return 0;
}
