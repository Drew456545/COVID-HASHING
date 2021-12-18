//
// Created by Drew Parker on 09/25/21.
//

#ifndef INTSET_H /* Prevent multiple inclusion... */
#define INTSET_H
class Intset {
private:
    int N; // number of elements currently stored in set
    int M; // size of array allocated for the set
    struct Node {
        int key;
        Node *next;
        Node (int k, Node *n) { key = k; next = n; }
    };
    Node **table;  // array of head pointers
    void free_table(void);
    int myhash(int key, int table_size);
    int myhash(int key);

public:
    Intset();
    ~Intset();
    bool find(int key);
    void insert(int key);
    void remove(int key);
    void print(void);
};
#endif


