#ifndef PART3_H
#define PART3_H

typedef struct cellKey {
	Key* data ;
	struct cellKey* next ;
} CellKey ;

typedef struct cellProtected {
	Protected* data ;
	struct cellProtected* next ;
} CellProtected ;

CellKey* create_cell_key(Key* key);
void insert_key(Key* key, CellKey** ck);
CellKey* read_public_keys(char *fichier);
void print_list_keys(CellKey* LCK);
void delete_cell_key(CellKey* c);
void delete_list_keys(CellKey* LCK);
CellProtected* create_cell_protected(Protected* pr);
void insert_protected(Protected* pr, CellProtected** cp);
CellProtected* read_protected(char *fichier);
void print_list_protected(CellProtected* LCP);
void delete_cell_protected(CellProtected* c);
void delete_list_protected(CellProtected* LCP);
void delete_invalide(CellProtected** LCP);

typedef struct hashcell {
	Key * key ;
	int val ;
	} HashCell ;

typedef struct hashtable {
	HashCell ** tab ;
	int size ;
} HashTable ;

HashCell* create_hashcell(Key* key);
int hash_function(Key* key, int size);
int find_position(HashTable* t, Key* key);
HashTable* create_hashtable(CellKey* keys, int size);
void delete_hashtable(HashTable* t);
Key* compute_winner(CellProtected* decl, CellKey* candidates, CellKey* voters, int sizeC, int sizeV);

#endif
