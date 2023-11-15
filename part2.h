#ifndef PART2_H
#define PART2_H

typedef struct key_ {
	long val; // publique ou secrète
	long n;
} Key;

typedef struct signature_ {
	long *content;
	int size;
} Signature;

typedef struct prot {
	Key *pKey;
	char *mess;
	Signature *sgn;
} Protected;

void init_key(Key* key, long val, long n);
void init_pair_keys(Key* pKey, Key* sKey, long low_size, long up_size);
char* key_to_str(Key* key);
Key* str_to_key(char* str);
Signature* init_signature(long* content, int size);
Signature* sign(char* mess, Key* sKey);
char* signature_to_str(Signature* sgn);
Signature* str_to_signature(char* str);
Protected* init_protected(Key* pKey, char* mess, Signature* sgn);
int verify(Protected* pr);
char* protected_to_str (Protected* pr);
Protected *str_to_protected (char *str);

void generate_random_data(int nv, int nc);
void free_signature(Signature *s);
void free_protected(Protected *pr);

#endif
