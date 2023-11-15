#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "part1.h"
#include "part2.h"
#include "part3.h"
#include "part4.h"
#include <openssl/sha.h>

void ecrire_bloc (Block *b, char *fichier) {
    // écrit dans un fichier un bloc
	FILE *f = fopen(fichier, "w");
	fprintf(f, "(%lx,%lx)\n", b->author->val, b->author->n);
    fprintf(f, "%s\n%s\n%d\n", b->hash, b->previous_hash, b->nonce);
	CellProtected *tmp = b->votes;
    char *chaine;
	while (tmp != NULL) {
        chaine = protected_to_str(tmp->data);
		fprintf(f, "%s \n", chaine);
        free(chaine);
		tmp = tmp->next;
	}
	fclose(f);
}

Block* lire_bloc (char *fichier) {
    // lit un bloc depuis un fichier
	FILE *f = fopen(fichier, "r");
    if (f == NULL) { // on vérifie que l'ouverture du fichier a réussi
		printf("Erreur lors de l'ouverture du fichier\n");
		return NULL;
	}
	Block *b = (Block*)malloc(sizeof(Block));
    if (b == NULL) { // on vérifie si l'allocation à fonctionné
        printf("Erreur lors de l'allocation");
        return NULL;
    }
	char ligne[256];
	fgets(ligne, 256, f);
	b->author = str_to_key(ligne);
    fgets(ligne, 256, f);
    ligne[strlen(ligne)-1] = '\0'; // on enlève le retour à la ligne pour hash
    b->hash = strdup(ligne);
    fgets(ligne, 256, f);
    ligne[strlen(ligne)-1] = '\0'; // on enlève le retour à la ligne pour previous_hash
    b->previous_hash = strdup(ligne);
    fgets(ligne, 256, f);
    int n=0;
    sscanf(ligne, "%d ", &n);
    b->nonce=n;
    fgets(ligne, 256, f);
	CellProtected* cp = create_cell_protected(str_to_protected(ligne));
	CellProtected* tmp = cp;
	while (fgets(ligne, 256, f)) {
		tmp->next = create_cell_protected(str_to_protected(ligne)); // on ajoute les déclarations de vote les unes après les autres, pour avoir un bloc identique à celui qu'on a utiliser pour écrire le bloc
		tmp = tmp->next;
	}
    b->votes = cp;
    fclose(f);
	return b;
}

char* block_to_str(Block* block) {
    // géenère une chaîne de caractères représentant un bloc
    char *k = key_to_str(block->author);
    char *str = (char*)malloc(sizeof(char)*256);
    if (str == NULL) { // on vérifie si l'allocation à fonctionné
        printf("Erreur lors de l'allocation");
        return NULL;
    }
    sprintf(str, "%s %s ", k, block->previous_hash);
    CellProtected *tmp = block->votes;
    char *chaine;
    int l = 256; // longueur de str
	while (tmp != NULL) {
        chaine = protected_to_str(tmp->data);
        l += strlen(chaine);
        str = realloc(str, l*sizeof(char)); // on réalloue str pour qu'elle puisse contenir la déclaration en plus
        strcat(str, chaine);
        free(chaine);
		tmp = tmp->next;
	}
    char nb[10];
    sprintf(nb, " %d", block->nonce);
    strcat(str, nb);
    free(k);
	return str;
}

unsigned char* hachage_sha256(char *chaine) {
    //  fonction qui prend une chaîne de caractères, et retourne sa valeur hachée obtenue par l’algorithme SHA256
    unsigned char *str = malloc(sizeof(unsigned char)*256);
    if (str == NULL) { // on vérifie si l'allocation à fonctionné
        printf("Erreur lors de l'allocation");
        return NULL;
    }
    str[0] = '\0';
	unsigned char *d = SHA256(chaine,strlen(chaine),0);
    char c[256];
	for (int i=0; i<SHA256_DIGEST_LENGTH; i++) {
        sprintf(c, "%02x", d[i]); // conversion en entier hexadécimal
        strcat(str, c);
	}
	return str;
}

void compute_proof_of_work(Block *B, int d) {
    // fonction incrémente l’attribut nonce jusqu'à ce que la valeur hachée du bloc commence par d zéros successifs
    char *chaine = block_to_str(B);
    unsigned char* hach = hachage_sha256(chaine);
    char debut[d+1]; // les d premiers caractères de hash
    char zero[d+1]; // chaîne contenant d zéros successifs
    for (int i=0; i<d; i++) {
        debut[i] = hach[i];
        zero[i] = '0';
    }
    debut[d]='\0';
    zero[d]='\0';
    while (strcmp(debut, zero) != 0) {
        B->nonce++;
        free(chaine);
        free(hach);
        chaine = block_to_str(B); // on recalcule block_to_str avec le nouveau nonce
        hach = hachage_sha256(chaine);
        for (int i=0; i<d; i++) {
            debut[i] = hach[i];
        }
        debut[d]='\0';
    }
    free(chaine);
    free(hach);
}

int verify_block(Block* B, int d) {
    // vérifie qu'un bloc est valide, c'est-à-dire que la valeur hachée du bloc commence par d zéros successifs
    char *chaine = block_to_str(B);
    unsigned char* hach = hachage_sha256(chaine);
    char debut[d+1]; // les d premiers caractères de hash
    char zero[d+1]; // chaîne contenant d zéros successifs
    for (int i=0; i<d; i++) {
        debut[i] = hach[i];
        zero[i] = '0';
    }
    debut[d]='\0';
    zero[d]='\0';
    free(chaine);
    free(hach);
	return (strcmp(debut, zero) == 0);
}

void delete_block(Block *B) {
    // Supprime un bloc
    if (B != NULL) {
        CellProtected* tmp;
        while (B->votes != NULL) {
            tmp = B->votes;
            B->votes = B->votes->next;
            free(tmp);
        }
        if (B->previous_hash != NULL) {
            free(B->previous_hash);
        }
        if (B->hash != NULL) {
            free(B->hash);
        }
        free(B);
    }
}
