

#ifndef LOCMAP_H /* Prevent multiple inclusion... */
#define LOCMAP_H
struct Location {
    int seq, index;
    Location *next;
    Location (int s, int i, Location *n) { seq = s; index = i; next = n; }
};
void free_locs(Location *L);
Location *copy_locs(Location *L);
// maps ints to hashesh
class Locmap {
private:
    int N; // number of elements currently stored in set
    int M; // size of array allocated for the set
    struct Node {
        int key;
        Location *locs;
        Node *next;
        Node (int k, Node *n) { key = k; next = n; locs = NULL; }
    };
    Node **table;  // array of head pointers
    void free_table(void);
    int myhash(int key, int table_size);
    int myhash(int key);
    bool find(int key);
    void insert(int key);

public:
    Locmap();
    ~Locmap();
    Location *&operator[] (int key);
};
#endif


