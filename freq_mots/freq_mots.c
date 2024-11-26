#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORD_SIZE 32
#define MAX_WORDS 1024

struct Word {
    char word[MAX_WORD_SIZE];
    int frequence;
};

struct Word table[MAX_WORDS];
int tableSize;

void parse_file(char* path);
void add_word(char* word);
int cmp_words(const void* ptrA, const void* ptrB);

void parse_file(char* path) {
    FILE* p;
    p = fopen(path, "r");
    if (p == NULL)
        exit(EXIT_FAILURE);
    char currentWord[MAX_WORD_SIZE];
    char current;
    int wIndex = 0; // index of the current word
    while ((current = fgetc(p)) != EOF) {
        if (current >= 'A' && current <= 'Z') {
            current = tolower(current);
            currentWord[wIndex] = current;
            wIndex++;
        } else if ((current >= 'a' && current <= 'z') || current == '-') {
            currentWord[wIndex] = current;
            wIndex++;
        } else if (wIndex > 0) { // length 0 is not a word
            add_word(currentWord);
            memset(currentWord, 0, MAX_WORD_SIZE);
            wIndex = 0;
            continue;
        }
    }
    fclose(p);
    return;
}

void add_word(char* word) {
    // check if the word exists
    for (int i=0; i<tableSize; i++) {
        if (strcmp(table[i].word, word)==0) {
            table[i].frequence++;
            return;
        }
    }
    // add the word
    strcpy(table[tableSize].word, word);
    table[tableSize].frequence = 1;
    tableSize++;
}

int cmp_words(const void* ptrA, const void* ptrB) {
    struct Word* A = (struct Word*) ptrA;
    struct Word* B = (struct Word*) ptrB;
    // in lexicographic order
    if (A->frequence == B->frequence)
        return strcmp(A->word, B->word);
    // most frequent first
    return  B->frequence - A->frequence;
}

void display() {
    printf("Word: appearence in text\n");
    for (int i=0; i<tableSize; i++) {
        printf("%s: %d\n", table[i].word, table[i].frequence);
    }
}

int main() {
    tableSize = 0;
    parse_file("file.txt");
    qsort(table, tableSize, sizeof(struct Word), cmp_words);
    display();
    return 0;
}
