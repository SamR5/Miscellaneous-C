#include <stdlib.h>
#include <stdbool.h>
#include "node.h"

struct Node* new_head(struct Node* head) {
    struct Node* newHead = malloc(sizeof(struct Node*));
    newHead->h = head->h;
    newHead->w = head->w;
    newHead->previous = head;
    return newHead;
}

struct Node* get_tail(struct Node* head) {
    struct Node** tmp = &head;
    while ((*tmp)->previous != NULL) {
        tmp = &(*tmp)->previous;
    }
    return *tmp;
}

void remove_tail(struct Node* head) {
    struct Node** tmp = &head;
    while ((*tmp)->previous != NULL) {
        tmp = &(*tmp)->previous;
    }
    *tmp = NULL;
    free(*tmp);
}

void check_edge(struct Node* head, int height, int width) {
    if (head->h == height)
        head->h = 0;
    if (head->w == width)
        head->w = 0;
    if (head->h < 0)
        head->h = height-1;
    if (head->w < 0)
        head->w = width-1;
}

bool bite_self(struct Node* head) {
    struct Node* tmp = head->previous;
    while (tmp != NULL) {
        if (tmp->h == head->h && tmp->w == head->w)
            return true;
        tmp = tmp->previous;
    }
    return false;
}

bool is_touching(struct Node* head, int h, int w) {
    struct Node* tmp = head;
    while (tmp != NULL) {
        if (tmp->h == h && tmp->w == w)
            return true;
        tmp = tmp->previous;
    }
    return false;
}

int get_size(struct Node* head) {
    int x = 0;
    struct Node* tmp = head;
    do {
        x++;
        tmp = tmp->previous;
    } while(tmp != NULL);
    return x;
}

void remove_snake(struct Node* head) {
    while (head->previous != NULL)
        remove_tail(head);
    free(head);
}
