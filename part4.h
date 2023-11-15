#ifndef PART4_H
#define PART4_H

typedef struct block {
	Key* author ;
	CellProtected* votes ;
	unsigned char* hash ;
	unsigned char* previous_hash ;
	int nonce ;
} Block ;

void ecrire_bloc (Block *b, char *fichier);
Block* lire_bloc (char *fichier);
char* block_to_str(Block* block);
unsigned char* hachage_sha256(char *chaine);
void compute_proof_of_work(Block *B, int d);
int verify_block(Block *B, int d);
void delete_block(Block *B);

#endif
