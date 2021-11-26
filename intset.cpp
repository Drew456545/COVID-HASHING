
#include <iostream>
#include "locmap.h"
using namespace std;
void free_locs(Location *L)
{
    while (L != NULL) {
        Location *to_delete = L;
        L = L->next;
        delete to_delete;
    }
}
Location *copy_locs(Location *L)
{
    return L== nullptr ? nullptr : new Location(L->seq, L->index, copy_locs(L->next));
}
Locmap::Locmap()
{
    N = 0;  // start with no elements present
    M = 100;  // start with a 4-element array
    table = new Node *[M]();
}
Locmap::~Locmap() { free_table(); }
/* Private function to de-allocate all table memory */
void Locmap::free_table(void)
{
    for (int i=0; i<M; i++)
        while (table[i] != NULL) {
            Node *to_delete = table[i];
            table[i] = table[i]->next;
            free_locs(to_delete->locs);
            delete to_delete;
        }
    delete [] table;
}
int Locmap::myhash(int key, int table_size)
{
    return ((unsigned)key * 1791321 + 76543) % table_size;
}
int Locmap::myhash(int key)
{
    return myhash(key, M);
}
/* Return true if key is in the set */
bool Locmap::find(int key)
{
    for (Node *n = table[myhash(key)]; n != NULL; n = n->next)
        if (n->key == key) return true;
    return false;
}
Location *&Locmap::operator[] (int key) {
    if (!find(key)) insert(key);
    Node *n = table[myhash(key)];
    while (n->key != key) n = n->next;
    return n->locs;
}
//Inserts a new key, if it isn't already present
void Locmap::insert(int key)
{
    if (find(key)) return;
    // Enlarge (by 2x) the underlying array if it isn't large enough to preserve mem
    if (N == M) {
        Node **newtbl = new Node *[M*2]();
        for (int i=0; i<M; i++)
            for (Node *n = table[i]; n != NULL; n = n->next) {
                int h = myhash(n->key, M*2);
                newtbl[h] = new Node(n->key, newtbl[h]);
                newtbl[h]->locs = copy_locs(n->locs);
            }
        free_table();
        M *= 2;
        table = newtbl;
    }
    N++;
    int h = myhash(key);
    table[h] = new Node(key, table[h]);
}

