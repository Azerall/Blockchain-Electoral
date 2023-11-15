#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "part1.h"
#include "part2.h"

void init_key(Key* key, long val, long n) {
    // initialise une clé déjà allouée
	key->val = val;
	key->n = n;
}

void init_pair_keys(Key* pKey, Key* sKey, long low_size, long up_size) {
    // utilise le protocole RSA pour initialiser une clé publique et une clé secrète (déjà allouéees)
	//Generation de cle :
    long p = random_prime_number(low_size,up_size,5000);
    long q = random_prime_number(low_size,up_size,5000);
    while (p == q) {
        q = random_prime_number(low_size,up_size,5000);
    }
    long n, s, u;
    generate_key_values(p, q, &n, &s, &u);
    //Pour avoir des cles positives :
    if (u<0) {
        long t = (p-1)*(q-1);
        u = u+t; //on aura toujours s*u mod t = 1
    }
    init_key(pKey, u, n);
    init_key(sKey, s, n);
}

char* key_to_str(Key* key) {
    // permet de passer d'une variable de type Key à sa représentation sous forme de chaîne de caractères
    if (key==NULL) {
    	return "";
    }
	char *str = (char*)malloc(sizeof(char)*256);
	sprintf(str, "(%lx,%lx)", key->val, key->n);
	return str;
}

Key* str_to_key(char* str) {
    // permet de passer d'une variable en forme de chaîne de caractères à sa représentation sous type Key
    if (str==NULL) {
	    return NULL;
    }
	Key *key = malloc(sizeof(Key));
    long val, n;
	sscanf(str, "(%lx,%lx)", &val, &n);
    init_key(key, val, n);
	return key;
}

Signature* init_signature(long* content, int size) {
    // permet d'allouer et de remplir une signature avec un tableau de long déjà alloué et initialisé
	Signature *s = malloc(sizeof(Signature));
	s->content = content;
	s->size = size;
	return s;
}

Signature* sign(char* mess, Key* sKey) {
    // crée une signature à partir du message mess (déclaration de vote) et de la clé secrète de l'émetteur
	long *content = encrypt(mess, sKey->val, sKey->n);
	return init_signature(content, strlen(mess));
}

char* signature_to_str(Signature* sgn) {
    // permet de passer d'une Signature à sa représentation sous forme de chaîne de caractères
	char* result = malloc(10*sgn->size*sizeof(char ));
	result[0]= '#' ;
	int pos = 1;
	char buffer[156];
	for (int i=0; i<sgn->size; i++) {
		sprintf(buffer, "%lx", sgn->content[i]);
		for (int j=0; j<strlen(buffer); j++) {
			result[pos] = buffer[j];
			pos = pos+1;
		}
		result[pos] = '#' ;
		pos = pos+1;
	}
	result[pos] = '\0' ;
	result = realloc(result, (pos+1)*sizeof(char));
	return result;
}

Signature* str_to_signature(char* str) {
    // permet de passer d'une de chaîne de caractères à sa représentation sous forme de Signature
	int len = strlen(str);
	long* content = (long*)malloc(sizeof(long)*len);
	int num = 0;
	char buffer[256];
	int pos = 0;
	for (int i=0; i<len; i++) {
		if (str[i] != '#' ) {
			buffer[pos] = str[i];
			pos = pos+1;
		} else {
			if (pos != 0) {
				buffer[pos] = '\0' ;
				sscanf(buffer, "%lx", &(content[num]));
				num = num+1;
				pos = 0;
			}
		}
	}
	content = realloc(content, num* sizeof(long));
	return init_signature(content, num);
}

void free_signature(Signature *s) {
    // désalloue une signature
    if (s != NULL) {
        if (s->content != NULL) {
            free(s->content);
        }
        free(s);
    }
}

Protected* init_protected(Key* pKey, char* mess, Signature* sgn) {
    // alloue et initialise une structure Protected
	Protected *p = malloc(sizeof(Protected));
    if (p == NULL) { // on vérifie si l'allocation à fonctionné
        printf("Erreur lors de l'allocation");
        return NULL;
    }
	p->pKey = pKey;
	p->mess = mess;
	p->sgn = sgn;
	return p;
}

int verify(Protected* pr) {
    // vérifie que la signature contenue dans pr correspond bien au message et à la personne contenus dans pr
	char *msg = decrypt(pr->sgn->content, pr->sgn->size, pr->pKey->val, pr->pKey->n);
    int i = strcmp(msg,pr->mess);
    free(msg);
	return (i == 0);
}

char* protected_to_str (Protected* pr) {
    // passe d’un Protected à sa représentation sous forme de chaîne de caractères
	char *str = malloc(sizeof(char)*256);
    if (str == NULL) { // on vérifie si l'allocation à fonctionné
        printf("Erreur lors de l'allocation");
        return NULL;
    }
    char *k = key_to_str(pr->pKey);
    char *s = signature_to_str(pr->sgn);
	sprintf(str, "%s %s %s", k, pr->mess, s);
    free(k);
    free(s);
	return str;
}

Protected *str_to_protected (char *str) {
    // passe d’une chaîne de caractères à sa représentation sous forme de Protected
    char key[256], sgn[256];
    char *mess = malloc(sizeof(char)*256);
	sscanf(str, "%s %s %s ", key, mess, sgn);
    Key *k = str_to_key(key);
    Signature* s = str_to_signature(sgn);
	return init_protected(k, mess, s);
}

void free_protected(Protected *pr) {
    // désalloue un Protected
    if (pr != NULL) {
        if (pr->pKey != NULL) {
            free(pr->pKey);
        }
        if (pr->mess != NULL) {
            free(pr->mess);
        }
        if (pr->sgn != NULL) {
            free_signature(pr->sgn);
        }
        free(pr);
    }
}

void generate_random_data(int nv, int nc) {
    // fonction de simulation d'un vote
    Key* pKey = malloc(sizeof(Key));
    Key* sKey = malloc(sizeof(Key));
    if (pKey == NULL || sKey == NULL) { // on vérifie si l'allocation à fonctionné
        printf("Erreur lors de l'allocation");
        return;
    }
    int r;
    char buffer[256];

    FILE *f1 = fopen("keys.txt", "w+");
    long* tab1 = malloc(sizeof(long)*nv*3); // on souhaite stocker p, s et n de chaque clé
    for(int j=0; j<nv*3; j++) {
        tab1[j]=0;
    }
    int i=0;
    int bool;
    while (i<nv) {
        bool=0;
        init_pair_keys(pKey, sKey, 3, 7);
        for (int j=0; j<i; j++) { // on vérifie si la personne existe déjà
            if (tab1[3*j] == pKey->val && tab1[3*j+1] == sKey->val && tab1[3*j+2] == pKey->n) {
                bool=1;
                break;
            }
        }
        if (bool == 1) { // si la personne existe déjà, on recommence
            continue;
        }
        tab1[3*i]=pKey->val;
        tab1[3*i+1]=sKey->val;
        tab1[3*i+2]=pKey->n;
        fprintf(f1, "(%lx,%lx) (%lx,%lx)\n", pKey->val, pKey->n, sKey->val, sKey->n);
        i++;
    }

    FILE *f2 = fopen("candidates.txt", "w+");
    char pub[256];
    char sec[256];
    int* tab2 = malloc(sizeof(int)*nc); // on souhaite stocker le numéro des lignes où se situe les candidats déjà sélectionnés
    for (int j=0; j<nc; j++) {
        tab2[j]=0;
    }
    i=0;
    while (i<nc) {
        bool=0;
        r = rand()%nv+1;
        for (int j=0; j<i; j++) { // on vérifie si le candidat a déjà été selectionné
            if (tab2[j] == r) {
                bool=1;
                break;
            }
        }
        if (bool == 1) { // si c'est le cas, on recommence
            continue;
        }
        tab2[i] = r;
        rewind(f1);
        for (int j=0; j<r; j++) {
            fgets(buffer, 256, f1);
        }
        sscanf(buffer, "%s %s ", pub, sec);
        fprintf(f2, "%s\n", pub);
        i++;
    }

    rewind(f1);
    FILE *f3 = fopen("declarations.txt", "w");
    char cand[256];
    Key *k;
    char *tmp;
    Signature* sgn;
    for (i=0; i<nv; i++) {
        fgets(buffer, 256, f1);
        sscanf(buffer, "%s %s ", pub, sec);
        r = rand()%nc+1;
        rewind(f2);
        for (int j=0; j<r; j++) {
            fgets(buffer, 256, f2);
        }
        sscanf(buffer, "%s ", cand);
        k = str_to_key(sec);
        sgn = sign(cand, k);
        tmp = signature_to_str(sgn);
        fprintf(f3, "%s %s %s\n", pub, cand, tmp);
        free(k);
        free_signature(sgn);
        free(tmp);
    }

    free(pKey);
    free(sKey);
    free(tab1);
    free(tab2);
    fclose(f1);
    fclose(f2);
    fclose(f3);
}
