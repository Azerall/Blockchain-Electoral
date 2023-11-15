#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "part1.h"
#include "part2.h"
#include "part3.h"

CellKey* create_cell_key(Key* key) {
	// fonction qui alloue et initialise une cellule de liste chaînée
	CellKey* ck = (CellKey*)malloc(sizeof(CellKey));
    if (ck == NULL) { // on vérifie si l'allocation à fonctionné
        return NULL;
    }
	ck->data = key;
	ck->next = NULL;
}

void insert_key(Key* key, CellKey** ck) {
    // fonction qui ajoute une clé en tête de liste
	CellKey *c = create_cell_key(key);
	c->next = *ck;
	*ck=c;
}

CellKey* read_public_keys(char *fichier) {
    // fonction qui retourne une liste chaînée contenant toutes les clés publiques du fichier.
	FILE *f = fopen(fichier, "r");
	if (f == NULL) { // on vérifie que l'ouverture du fichier a réussi
		printf("Erreur lors de l'ouverture du fichier\n");
		return NULL;
	}
	CellKey* ck = NULL;
	char ligne[256];
	char public[256];
	while (fgets(ligne, 256, f) != NULL) { // on parcourt tout le fichier
		if (sscanf(ligne, "%s %s ", public, ligne) >= 1) { // soit keys.txt qui contient 2 clés, soit candidates.txt qui contient 1 clé
			insert_key(str_to_key(public), &ck);
		} else { // problème avec le contenu du fichier, on annule tout
            delete_list_keys(ck);
            return NULL;
        }
	}
	fclose(f);
	return ck;
}

void print_list_keys(CellKey* LCK) {
    // fonction qui affiche une liste chaînée de clés
    char *chaine;
	while (LCK != NULL) {
        chaine = key_to_str(LCK->data);
		printf("%s\n", chaine);
        free(chaine);
		LCK=LCK->next;
	}
}

void delete_cell_key(CellKey* c) {
    // foncton qui supprime une cellule de liste chaînée de clés
	if (c != NULL) {
        if (c->data != NULL) {
            free(c->data);
        }
		free(c);
	}
}

void delete_list_keys(CellKey* LCK) {
    // foncton qui supprime une liste chaînée de clés
	CellKey *tmp = NULL;
	while (LCK != NULL) {
		tmp = LCK->next;
		delete_cell_key(LCK);
        LCK=tmp;
	}
}

CellProtected* create_cell_protected(Protected* pr) {
    // fonction qui alloue et initialise une cellule de liste chaînée
	CellProtected *cp = (CellProtected*)malloc(sizeof(CellProtected));
    if (cp == NULL) { // on vérifie si l'allocation à fonctionné
        return NULL;
    }
	cp->data = pr;
	cp->next = NULL;
	return cp;
}

void insert_protected(Protected* pr, CellProtected** cp) {
    // fonction qui ajoute une déclaration signée en tête de liste
	CellProtected *c = create_cell_protected(pr);
	c->next = *cp;
	*cp=c;
}

CellProtected* read_protected(char *fichier) {
    // fonction qui crée une liste contenant toutes les déclarations signées du fichier
	FILE *f = fopen(fichier, "r");
	if (f == NULL) {
		printf("Erreur lors de l'ouverture du fichier\n");
		return NULL;
	}
	CellProtected* cp = NULL;
	char ligne[256];
	while (fgets(ligne, 256, f) != NULL) {
		Protected *pr = str_to_protected(ligne);
		insert_protected(pr, &cp);
	}
	fclose(f);
	return cp;
}

void print_list_protected(CellProtected* LCP) {
    // fonction qui affiche une liste chaînée de déclarations signées
	while (LCP != NULL) {
        char *chaine = protected_to_str(LCP->data);
		printf("%s\n", chaine);
        free(chaine);
		LCP=LCP->next;
	}
}

void delete_cell_protected(CellProtected* c) {
    // fonction qui supprime une cellule de liste chaînée de déclarations signées
	if (c != NULL) {
        if (c->data != NULL) {
            free_protected(c->data);
        }
		free(c);
	}
}

void delete_list_protected(CellProtected* LCP) {
    // fonction qui supprime entièrement une liste chaînée de déclarations signées
	CellProtected *tmp;
	while (LCP != NULL) {
		tmp = LCP;
		LCP=LCP->next;
		delete_cell_protected(tmp);
	}
}

void delete_invalide(CellProtected** LCP) {
    // fonction qui supprime toutes les déclarations dont la signature n’est pas valide
    CellProtected *l = *LCP;
    if (l == NULL) {
        return;
    }
    while (verify(l->data) == 0) { // si la(es) toute(s) première(s) signature n'est(sont) pas valide(s)
        *LCP = l->next;
        free(l);
        l = *LCP; // la première signature étant supprimée, on change le pointeur sur la prochaine
    }
    CellProtected *tmp;
	while (l->next != NULL) { // on parcourt la liste
		tmp=l->next;
		if (verify(tmp->data) == 0) {
			l->next=tmp->next;
			delete_cell_protected(tmp);
		} else {
			l=l->next;
		}
	}
}

HashCell* create_hashcell(Key* key) {
    // alloue une cellule de la table de hachage, et qui initialise ses champs en mettant la valeur à zéro
	HashCell *hs = malloc(sizeof(HashCell));
	hs->key=key;
	hs->val=0;
	return hs;
}

int hash_function(Key* key, int size) {
    // retourne la position d’un élément dans la table de hachage
	return (key->val)%size;
}

int find_position(HashTable* t, Key* key) {
    // cherche dans la table t s’il existe un élément dont la clé publique est key
	int clef = hash_function(key, t->size);
	if (t->tab[clef] == NULL || (t->tab[clef]->key->val == key->val && t->tab[clef]->key->n == key->n)) {
		return clef;
	}
	int tmp=clef;
	clef++;
	while (clef != tmp) {
		if (clef >= t->size) { // on retourne au début de la table
			clef=0;
		}
		if (t->tab[clef] == NULL) {
			return clef;
		} else {
			if (t->tab[clef]->key->val == key->val && t->tab[clef]->key->n == key->n) {
				return clef;
			}
		}
		clef++;
	}
	return -1; // la personne n'est pas dans le tableau
}

HashTable* create_hashtable(CellKey* keys, int size) {
    // crée et initialise une table de hachage de taille size contenant une cellule pour chaque clé de la liste chaînée keys
	HashTable *ht = (HashTable*)malloc(sizeof(HashTable));
	ht->tab = malloc(sizeof(HashCell)*size);
	for (int i=0; i<size; i++) { // on initialise les cases du tableau
		ht->tab[i]=NULL;
	}
	int pos;
	ht->size=size;
	while (keys != NULL) {
		pos = find_position(ht, keys->data);
		ht->tab[pos] = create_hashcell(keys->data);
		keys=keys->next;
	}
	return ht;
}

void delete_hashtable(HashTable* t) {
    // supprime une table de hachage
    if (t != NULL) {
        if (t->tab != NULL) {
            for (int i=0; i<t->size; i++) {
                if (t->tab[i] != NULL)
                    free(t->tab[i]);
        	}
            free(t->tab);
        }
    	free(t);
    }

}

Key* compute_winner(CellProtected* decl, CellKey* candidates, CellKey* voters, int sizeC, int sizeV) {
    // calcule le vainqueur de l’élection
	HashTable *hc = create_hashtable(candidates, sizeC); // table de hachage contenant la liste des candidats
	HashTable *hv = create_hashtable(voters, sizeV); // table de hachage contenant la liste des voteurs
	int posV;
	int posC;
    Key *ckey;
	while (decl != NULL) {
		posV=find_position(hv, decl->data->pKey);
        ckey = str_to_key(decl->data->mess);
		posC=find_position(hc, ckey);
		if (posV != -1 && hv->tab[posV]->val == 0) { // la personne qui vote a le droit de voter et n’a pas déjà voté
			if (posC != -1) { // le candidat existe
				hc->tab[posC]->val++;
				hv->tab[posV]->val++;
			}
		}
        free(ckey);
		decl=decl->next;
	}
	int g=0; // position du gagant dans le tableau
	for (int i=0; i<sizeC; i++) {
		if (hc->tab[g]->val < hc->tab[i]->val) {
			g = i;
		}
	}
    Key *gagnant = malloc(sizeof(Key));
    init_key(gagnant, hc->tab[g]->key->val, hc->tab[g]->key->n);
	//printf("Nombre de vote : %d\n", hc->tab[g]->val);
    delete_hashtable(hc);
    delete_hashtable(hv);
	return gagnant;
}
