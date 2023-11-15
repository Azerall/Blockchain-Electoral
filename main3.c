#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "part1.h"
#include "part2.h"
#include "part3.h"

int main() {
	CellKey* cv = read_public_keys("keys.txt");
	CellKey* cc = read_public_keys("candidates.txt");
	CellProtected* cp = read_protected("declarations.txt");

	print_list_keys(cv);
	delete_invalide(&cp);
	print_list_protected(cp);

    Key* gagnant = compute_winner(cp, cc, cv, 50, 1000);
    char *chaine = key_to_str(gagnant);
	printf("Gagnant : %s\n", chaine);
    free(chaine);
    free(gagnant);

	delete_list_keys(cv);
	delete_list_keys(cc);
	delete_list_protected(cp);

	return 0;
}
