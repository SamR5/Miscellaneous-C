#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>

#include "node.h"

#define HEIGHT 10
#define WIDTH 10
#define TICK 1000*1000 // µs


enum direction {UP, RIGHT, DOWN, LEFT};
char* headShape[4] = {"^", ">", "v", "<"};
int hFood, wFood;

struct Node* move_snake(struct Node* head, int direction);
void add_food(struct Node* head);
bool food_eaten(struct Node* head);
void display(struct Node* head, int direction);
bool end_game(struct Node* head);
void setup();
struct Node* init_snake();

struct Node* move_snake(struct Node* head, int direction) {
    struct Node* newHead = new_head(head);
    switch (direction) {
        case UP:
            newHead->h = head->h-1;
            break;
        case RIGHT:
            newHead->w = head->w+1;
            break;
        case DOWN:
            newHead->h = head->h+1;
            break;
        case LEFT:
            newHead->w = head->w-1;
            break;
    }
    check_edge(newHead, HEIGHT, WIDTH);
    if (food_eaten(newHead)) {
        add_food(newHead);
    } else {
        remove_tail(newHead);
    }
    return newHead;
}

void add_food(struct Node* head) {
    if (get_size(head) == HEIGHT*WIDTH) {
        hFood = -1;
        wFood = -1;
        return;
    }
    do {
        hFood = HEIGHT * random() / RAND_MAX;
        wFood = WIDTH * random() / RAND_MAX;
    } while (is_touching(head, hFood, wFood));
}

bool food_eaten(struct Node* head) {
    return head->h == hFood && head->w == wFood;
}

void display(struct Node* head, int direction) {
    clear();
    for (int i=0; i<WIDTH*2+2; i++) mvprintw(0, i, "-");
    struct Node* current = head->previous;
    while (current != NULL) {
        mvprintw(current->h+1, 2*current->w+1, "#");
        current = current->previous;
    }
    mvprintw(hFood+1, 2*wFood+1, "@");
    mvprintw(head->h+1, 2*head->w+1, headShape[direction]);
    for (int i=0; i<WIDTH*2+2; i++) mvprintw(HEIGHT+1, i, "-");
    // 2 side bar
    for (int i=1; i<HEIGHT+1; i++) {
        mvprintw(i, 0, "|");
        mvprintw(i, WIDTH*2+1, "|");
    }
    refresh();
}

bool end_game(struct Node* head) {
    return bite_self(head) || get_size(head)==WIDTH*HEIGHT;
}

void setup() {
    (void) initscr();      /* initialize the curses library */
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    (void) nonl();         /* tell curses not to do NL->CR/NL on output */
    (void) cbreak();       /* take input chars one at a time, no wait for \n */
    (void) noecho();       /* echo input - in color */
    intrflush(stdscr, FALSE);
    nodelay(stdscr, TRUE);
    timeout(0);
    //curs_set(0);
}

struct Node* init_snake() {
    struct Node *snakeHead = malloc(sizeof(struct Node*));
    snakeHead->h = 1;
    snakeHead->w = 1;
    return snakeHead;
}


int main(int argc, char* argv[]) {
    srand(time(0));
    setup();

    struct Node* snakeHead = init_snake();
    add_food(snakeHead);
    int direction = RIGHT;

    struct timeval tv1, tv2;
    gettimeofday(&tv1,NULL);
    gettimeofday(&tv2,NULL);
    bool skipAutoMove;

    display(snakeHead, direction);

    while (!end_game(snakeHead)) {
        //int c = getch();
        int c = wgetch(stdscr);
        // if the player press a key, skip the part where the snake moves automatically
        // otherwise the snake moves every 'TICK'
        skipAutoMove = true;
        switch (c) {
            case KEY_UP:
                direction = UP;
                break;
            case KEY_DOWN:
                direction = DOWN;
                break;
            case KEY_RIGHT:
                direction = RIGHT;
                break;
            case KEY_LEFT:
                direction = LEFT;
                break;
            default:
                skipAutoMove = false;
        }
        if (skipAutoMove) {
            snakeHead = move_snake(snakeHead, direction);
            // reseting the timer is important to restart the automatic move
            // every keystroke
            gettimeofday(&tv1, NULL);
            display(snakeHead, direction);
            continue;
        }

        // if the interval since the last display < TICK, move and display automatically
        gettimeofday(&tv2, NULL);
        if (tv2.tv_sec*1000000+tv2.tv_usec - tv1.tv_sec*1000000-tv1.tv_usec > TICK) {
            gettimeofday(&tv1, NULL);
            snakeHead = move_snake(snakeHead, direction);
            display(snakeHead, direction);
        }
        usleep(20*1000);
    }
    clear();
    int ss = get_size(snakeHead);
    if (ss == HEIGHT*WIDTH)
        mvprintw(4, 1, "You won with snake size: %d\n", ss);
    else
        mvprintw(4, 1, "You lost with snake size: %d\n", ss);
    refresh();
    usleep(3000*1000);
    remove_snake(snakeHead);
    return 0;
}
