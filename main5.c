#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "part1.h"
#include "part2.h"
#include "part3.h"
#include "part4.h"
#include "part5.h"

#define NB_VOT 1000
#define NB_CAND 5

int main() {
    srand(time(NULL));
    generate_random_data(NB_VOT, NB_CAND);
    CellKey* cv = read_public_keys("keys.txt");
	CellKey* cc = read_public_keys("candidates.txt");
	CellProtected* cp = read_protected("declarations.txt");
    system("exec rm -r ./Blockchain/*");

    CellProtected* tmp = cp;
    int i=0;
    CellTree* tree = NULL;
    char name[256];
    while (tmp != NULL) {
        submit_vote(tmp->data);
        i++;
        if (i%10 == 0) {
            create_block(tree, tmp->data->pKey, 2);
            if (tree == NULL) {
                tree = create_node(lire_bloc("Pending_block"));
            } else {
                add_child(last_node(tree), create_node(lire_bloc("Pending_block")));
            }
            sprintf(name, "%s_%d", "block",i);
            add_block(2, name);
            if (rand()%4==1 && i>10) { // 1 chance sur 4 d'avoir un bloc frauduleux ajouté
                Block *b = (Block*)malloc(sizeof(Block));
            	b->author = cc->data;
            	b->votes = last_node(tree)->block->votes;
                b->hash = NULL;
                b->previous_hash = last_node(tree)->father->block->hash;
                b->nonce = 0;
                char *chaine = block_to_str(b);
                b->hash = hachage_sha256(chaine);
                compute_proof_of_work(b,2);
                free(chaine);
                sprintf(name, "%s_%d%c", "./Blockchain/block",i,'a');
                ecrire_bloc(b, name);
                free(b->hash);
                free(b);
            }
        }
        if (tmp->next == NULL) { // s'il s'agit de la dernière déclaration
            FILE *f = fopen("Pending_votes.txt", "r"); // on vérifie s'il reste des votes qui n'ont pas été comptabilisés (dans le cas où le nombre de voteurs n'est pas un multiple de 10)
            if (f != NULL) {
                fclose(f);
                create_block(tree, tmp->data->pKey, 2); // l'auteur du dernier bloc est celui qui crée le bloc avec le reste des déclarations
                add_child(last_node(tree), create_node(lire_bloc("Pending_block")));
                sprintf(name, "%s_%d", "block",i);
                add_block(2, name);
            }
        }
        tmp=tmp->next;
    }

    CellTree* final_tree = read_tree();
    print_tree(final_tree);
    Key* gagnant = compute_winner_BT(final_tree, cc, cv, NB_CAND, NB_VOT);
    char *chaine = key_to_str(gagnant);
    printf("Gagnant : %s\n", chaine);

    free(chaine);
    free(gagnant);

    delete_tree(tree, 1, 1);
    delete_tree(final_tree, 1, 1);

    delete_list_keys(cv);
	delete_list_keys(cc);
	delete_list_protected(cp);

	return 0;
}
