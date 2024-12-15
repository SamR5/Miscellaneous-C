
#ifndef NODE_H
#define NODE_H

struct Node {
    int h, w;
    struct Node* previous;
};

struct Node* new_head(struct Node* head); // return a new node linked to 'head'
struct Node* get_tail(struct Node* head);
void remove_tail(struct Node* snake);
void check_edge(struct Node* head, int height, int width); // circular
bool bite_self(struct Node* head);
bool is_touching(struct Node* head, int h, int w);
int get_size(struct Node* head);
void remove_snake(struct Node* head);

#endif
