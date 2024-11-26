#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>


const char* path = "liste_des_mots.txt";
char* word; // the word to find
bool letters[26] = {0};

int lines_in_file(const char* path);
int random_range(int a, int b);
void purge_stdin(char c);
char* choose_word(int lineNb);
void next_round();
bool is_win();
void display();
void init();
bool ask_continue();
int count_errors();


int lines_in_file(const char* path) {
    FILE *p;
    p = fopen(path, "r");
    if (p==NULL)
        exit(EXIT_FAILURE);
    char* line = NULL;
    size_t len = 0;
    int count = 0;
    while (getline(&line, &len, p) != -1)
        count++;
    fclose(p);
    return count;
}

int random_range(int a, int b) {
    if (a>b) {
        int tmp = a;
        a = b;
        b = tmp;
    }
    float rr = (float)rand()/RAND_MAX; // random between 0 and 1
    return a + (b-a)*rr;
}

void purge_stdin(char lastC) {
    char tmp = lastC;
    while (tmp!='\n')
        tmp = getchar();
}

char* choose_word(int lineNb) {
    FILE *p;
    p = fopen(path, "r");
    if (p==NULL)
        exit(EXIT_FAILURE);
    char* line = NULL;
    size_t len = 0;
    int count=0;
    do {
        getline(&line, &len, p);
        count++;
    } while (count!=lineNb);

    line[strlen(line)-2] = '\0'; //'\n' at -2
    fclose(p);
    return line;
}

void next_round() {
    // display the letters entered
    printf("Entered letters: ");
    for (int i=0; i<26; i++) {
        if (letters[i])
            printf("%c", 'a'+i);
    }
    printf("\n");
    // ask user a letter
    char letter;
    printf("Enter a letter: ");
    letter = getchar();
    purge_stdin(letter); // in case user enter a string
    printf("\n");
    system("clear");
    // check the letter
    int index = tolower(letter) - 'a';
    if (index<0 || index>25)
        return;
    letters[index] = true;
}

bool is_win() {
    for (char* i = word; *i!='\0'; i++) {
        if (letters[*i-'a'] == false)
            return false;
    }
    return true;
}

void display() {
    char* i = word;
    printf("\tWelcome to the game: Find the word\n\n");
    while (*i!='\0') {
        if (letters[*i-'a']) {
            printf("%c", *i);
        } else {
            printf("*");
        }
        i++;
    }
    printf("\n");
}

void init() {
    int lines = lines_in_file(path);
    int rand = random_range(0, lines);
    word = choose_word(rand);
    memset(letters, false, 26);
}

bool ask_continue() {
    do {
        printf("Do you want to retry (y/n)? ");
        char answer = getchar();
        purge_stdin(answer);
        if (tolower(answer) == 'y')
            return true;
        else if (tolower(answer) == 'n')
            return false;
    } while (1);
}

int count_errors() {
    // count only the letters that didn't appear in word
    char* i = word;
    while (*i!='\0') {
        letters[*i-'a'] = false;
        i++;
    }
    int count = 0;
    for (int x=0; x<26; x++) {
        if (letters[x])
            count++;
    }
    return count;
}

int main() {
    srand(time(NULL)); // initialize the random generator
    do {
        system("clear");
        init();
        display();
        while (!is_win()) {
            next_round();
            display();
        }
        printf("You won with %d errors!\n", count_errors());
    } while(ask_continue());
    return 0;
}
