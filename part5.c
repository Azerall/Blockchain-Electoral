#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "part1.h"
#include "part2.h"
#include "part3.h"
#include "part4.h"
#include "part5.h"
#include <openssl/sha.h>
#include <dirent.h>

CellTree* create_node(Block* b) {
    // fonction permettant de créer et initialiser un noeud avec une hauteur égale à zéro
	CellTree *ct = (CellTree*)malloc(sizeof(CellTree));
    if (ct == NULL) { // on vérifie si l'allocation à fonctionné
        return NULL;
    }
	ct->block = b;
	ct->father = NULL;
	ct->firstChild = NULL;
	ct->nextBro = NULL;
	ct->height = 0;
	return ct;
}

int max(int a, int b) {
    // fonction auxilière permettant de calculer le maximum entre a et b
    if (a>=b) return a;
    else return b;
}

int update_height(CellTree* father, CellTree* child) {
    // fonction permettant de mettre à jour la hauteur du noeud father quand l’un de ses fils a été modifié
	if (father == NULL) {
		return 0;
	}
	int h = max(father->height, child->height+1);
	if (h == father->height) {
		return 0;
	} else {
		father->height = h;
		return 1;
	}
}

void add_child(CellTree* father, CellTree* child) {
    // ajoute un fils à un noeud, en mettant à jour la hauteur de tous les ascendants
    CellTree *tmp = father->firstChild;
    if (tmp == NULL) {
        father->firstChild = child;
    } else {
        while (tmp->nextBro != NULL) {
    		tmp = tmp->nextBro;
    	}
    	tmp->nextBro = child;
    }
    child->father = father;
    while (father != NULL) { // on parcourt tous les ascendants pour mettre à jour leur hauteur
        update_height(father, child);
        child = father;
        father = father->father;
    }
}

void print_noeud(CellTree* tree, int etage) {
    // fonction auxilière qui permet d'afficher un noeud et retenir sa profondeur
	if (tree != NULL) {
        for (int i=0; i<etage; i++) {
            printf("  ");
        }
		printf("%d %s\n", tree->height, tree->block->hash);
        print_noeud(tree->firstChild, etage+1);
		print_noeud(tree->nextBro, etage);
	}
}

void print_tree(CellTree* tree) {
    // fonction qui affiche un arbre
	print_noeud(tree, 0);
}

void delete_node(CellTree* node) {
    // supprime un noeud de l'arbre
    if (node != NULL) {
        if (node->block != NULL) {
            delete_block(node->block);
        }
        free(node);
    }
}

void delete_tree(CellTree* tree, int bool_aut, int bool_votes) {
    // supprime l'arbre,
    // si bool_aut = 1, on supprime également l'auteur
    // si bool_votes = 1, on supprime également le contenu des votes
	if (tree != NULL) {
		delete_tree(tree->nextBro, bool_aut, bool_votes);
        delete_tree(tree->firstChild, bool_aut, bool_votes);
        if (bool_aut == 1 && tree->block != NULL) {
            free(tree->block->author);
        }
        if (bool_votes == 1 && tree->block != NULL) {
            delete_list_protected(tree->block->votes);
            tree->block->votes = NULL;
        }
		delete_node(tree);
	}
}

CellTree* highest_child(CellTree* cell) {
    //  renvoie le noeud fils avec la plus grande hauteur
    if (cell == NULL) return NULL;
	CellTree* highest = cell->firstChild;
    if (highest == NULL) return NULL;
	CellTree* tmp = highest->nextBro;
	while (tmp != NULL) { // on parcourt tous ses frères
		if (tmp->height > highest->height) {
			highest = tmp;
		}
		tmp = tmp->nextBro;
	}
	return highest;
}

CellTree* last_node(CellTree* tree) {
    // fonction permettant de retourner le dernier bloc de cette plus longue chaîne.
    if (tree == NULL) {
        return NULL;
    }
    CellTree* ct = tree;
	while (ct->firstChild != NULL) {
		ct = highest_child(ct);
	}
	return ct;
}

void fusion_decl(CellProtected **cp1, CellProtected **cp2) {
    // fonction permettant de fusionner deux listes chaînées de déclarations signées
    // la première liste contiendra la deuxième, et la deuxième vaudra NULL
	if (*cp1 == NULL) {
        *cp1 = *cp2;
        *cp2 = NULL;
    } else if (*cp2 != NULL) {
        CellProtected *tmp = *cp1;
        while (tmp->next != NULL) {
    		tmp = tmp->next;
    	}
    	tmp->next = *cp2;
        *cp2 = NULL;
    }
}

CellProtected* fusion_plus_longue (CellTree *tree) {
    // retourne la liste obtenue par fusion des listes chaînées de déclarations contenues dans les blocs de la plus longue chaîne
	CellProtected *cp = NULL;
	while (tree != NULL) {
		fusion_decl(&cp, &(tree->block->votes));
		tree = highest_child(tree);
	}
	return cp;
}

void submit_vote(Protected* p) {
    // focntion permettant à un citoyen de soumettre un vote, autrement d'ajouter son vote à la fin du fichier "Pending votes.txt".
	FILE *f = fopen("Pending_votes.txt", "a"); // on souhaite ajouter à la fin du fichier
    if (f == NULL) { // on vérifie que l'ouverture du fichier a réussi
		printf("Erreur lors de l'ouverture du fichier\n");
		return;
	}
    char *chaine = protected_to_str(p);
	fprintf(f, "%s\n", chaine);
    free(chaine);
	fclose(f);
}

void create_block(CellTree* tree, Key* author, int d) {
    // - crée un bloc valide contenant les votes en attente dans le fichier "Pending votes.txt",
    // - supprime le fichier "Pending votes.txt après avoir créé le bloc,
    // - et écrit le bloc obtenu dans un fichier appelé "Pending block".
	Block *b = (Block*)malloc(sizeof(Block));
    if (b == NULL) { // on vérifie si l'allocation à fonctionné
        return;
    }
    b->author = author;
	b->votes = read_protected("Pending_votes.txt");
    b->hash = NULL;
    if (tree != NULL) {
        b->previous_hash = last_node(tree)->block->hash;
    } else {
        b->previous_hash = NULL;
    }
    b->nonce = 0;
    char *chaine = block_to_str(b);
    b->hash = hachage_sha256(chaine);
    compute_proof_of_work(b,d);
    free(chaine);
    remove("Pending_votes.txt");
    ecrire_bloc(b, "Pending_block");
    delete_list_protected(b->votes);
    b->previous_hash = NULL;
    b->votes = NULL;
    delete_block(b);
}

void add_block(int d, char* name) {
    // vérifie que le bloc représenté par le fichier "Pending block" est valide.
    // Si c’est le cas, la fonction crée un fichier appelé name représentant le bloc, puis l’ajoute dans le répertoire "Blockchain".
    // Dans tous les cas, le fichier "Pending block" est ensuite supprimé.
    Block *b = lire_bloc("Pending_block");
    if (verify_block(b, d)) {
        char chemin[256] = "./Blockchain/";
        strcat(chemin, name);
        ecrire_bloc(b, chemin);
    }
    remove("Pending_block");
    free(b->author); // on libère le bloc lu
    delete_list_protected(b->votes);
    b->votes = NULL;
    delete_block(b);
}

CellTree* read_tree() {
    // construit l’arbre correspondant aux blocs contenus dans le répertoire "Blockchain"
    int nb_fic = 0; // on compte d'abord le nombre de fichier dans le dossier
    DIR *rep = opendir("./Blockchain/");
    struct dirent *dir;
    if (rep != NULL) {
        while ((dir = readdir(rep))) {
            if (strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0) {
                nb_fic++;
            }
        }
        closedir(rep);
    }

    CellTree* T[nb_fic]; // tableau qui stockera tous les noeuds
    int i=0;
    rep = opendir("./Blockchain/");
    char name[512];
    if (rep != NULL) {
        while ((dir = readdir(rep))) {
            if (strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0) {
                sprintf(name, "./Blockchain/%s", dir->d_name); // on récupère le nom du fichier avec le chemin
                T[i] = create_node(lire_bloc(name)); // on l'ajoute le bloc du fichier au tableau
                i++;
            }
        }
        closedir(rep);
    }

    for (int i=0; i<nb_fic; i++) {
        for (int j=0; j<nb_fic; j++) {
            if (strcmp(T[j]->block->previous_hash,T[i]->block->hash) == 0) {
                 add_child(T[i], T[j]);
            }
        }
    }
    CellTree* test;
    for (int i=0; i<nb_fic; i++) {
        if (T[i]->father == NULL) {
            return T[i];
        }
    }
}

Key* compute_winner_BT(CellTree* tree, CellKey* candidates, CellKey* voters, int sizeC, int sizeV) {
    // détermine le gagnant de l’élection en se basant sur la plus longue chaîne de l’arbre
    CellProtected* cp = fusion_plus_longue(tree);
    delete_invalide(&cp);
    Key *k = compute_winner(cp, candidates, voters, sizeC, sizeV);
    delete_list_protected(cp);
    return k;
}
