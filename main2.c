#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "part1.h"
#include "part2.h"

int main() {

    srand (time(NULL)) ;

	//Testing Init Keys
	Key* pKey = malloc(sizeof(Key));
	Key* sKey = malloc(sizeof(Key));
	init_pair_keys(pKey, sKey, 3, 7);
	printf("pKey : %lx , %lx \n", pKey->val, pKey->n);
	printf("sKey : %lx , %lx \n", sKey->val, sKey->n);

	//Testing Key Serialization
	char* chaine = key_to_str(pKey);
	printf("key_to_str: %s \n", chaine);
	Key* k = str_to_key(chaine);
	printf("str_to_key: %lx , %lx \n", k->val, k->n) ;

	//Testing signature
	//Candidate keys:
	Key* pKeyC = malloc(sizeof(Key));
	Key* sKeyC = malloc(sizeof(Key));
	init_pair_keys(pKeyC, sKeyC, 3, 7);
	//Declaration:
	char* mess = key_to_str(pKeyC);
	printf("%s vote pour %s \n", chaine, mess);
	Signature* sgn = sign(mess, sKey);
	printf("signature : ");
	print_long_vector(sgn->content, sgn->size);
    free(chaine); // on libère la chaine d'avant
	chaine = signature_to_str(sgn);
	printf("signature to str : %s\n", chaine);
    free_signature(sgn); // on libère le sgn précédent
	sgn = str_to_signature(chaine);
	printf("str to signature : ");
	print_long_vector(sgn->content, sgn->size);


	//Testing protected:
	Protected* pr = init_protected(pKey, mess, sgn);
	//Verification:
	if (verify(pr)) {
		printf("Signature valide\n");
	} else {
		printf("Signature non valide\n");
	}
    free(chaine); // on libère la chaine d'avant
	chaine = protected_to_str(pr);
	printf("protected to str : %s \n" , chaine);
    free(pr); // on libère le pr précédent (sans son contenu)
	pr = str_to_protected(chaine);
    char *chaine2 = key_to_str(pr->pKey);
    free(chaine); // on libère la chaine d'avant
    chaine = signature_to_str(pr->sgn);
	printf("str to protected : %s %s %s \n", chaine2, pr->mess, chaine);

    generate_random_data(1000, 50);

    free(chaine);
    free(chaine2);
    free(k);
    free(mess);

    free(pKey);
	free(sKey);
	free(pKeyC);
	free(sKeyC);

    free_signature(sgn);
    free_protected(pr);

    return 0;
}
