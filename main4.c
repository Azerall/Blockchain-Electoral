#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "part1.h"
#include "part2.h"
#include "part3.h"
#include "part4.h"

#include <openssl/sha.h>

int main() {
	/*const char *s = "Rosetta code";
	unsigned char *d = SHA256(s,strlen(s),0);
	int i;
	for (i = 0; i < SHA256_DIGEST_LENGTH ; i++)
		printf ("%02x", d[i]);
	putchar('\n');

    unsigned char *test = hachage_sha256("Rosetta code");
    printf ("%s\n", test);
    free(test);*/

	Block *b = (Block*)malloc(sizeof(Block));
	b->author = str_to_key("(1b05,28a3)");
	CellProtected* cp = read_protected("votes.txt");
	b->votes = cp;
    b->hash = NULL;
    b->previous_hash = NULL;
    b->nonce = 0;
    char *chaine = block_to_str(b);
    b->hash = hachage_sha256(chaine);
    compute_proof_of_work(b,3);

    if (verify_block(b, 3)) printf("Bloc vérifié!\n");
    ecrire_bloc(b, "bloc.txt");

	// Etude du temps moyen de la fonction compute_proof_of_work
	clock_t temps_initial;
	clock_t temps_final;
	double temps_cpu;

    Block *tmp;
    FILE *f = fopen("compare_compute_proof_of_work.txt", "w");
	for (int i=0; i<=50; i++) {
        tmp = lire_bloc("bloc.txt");
		temps_initial = clock();
        compute_proof_of_work(tmp,i);
		temps_final = clock();
		temps_cpu = ((double)(temps_final-temps_initial))/CLOCKS_PER_SEC;
		fprintf(f, "%d %f\n", i, temps_cpu);
        if (temps_cpu > 1) {
            free(tmp->author);
            delete_list_protected(tmp->votes);
            tmp->votes = NULL;
            delete_block(tmp);
            break;
        }
        free(tmp->author);
        delete_list_protected(tmp->votes);
        tmp->votes = NULL;
        delete_block(tmp);
	}
	fclose(f);

    free(chaine);
    free(b->author);
    delete_list_protected(b->votes);
    b->votes = NULL;
    delete_block(b);

	return 0;
}
