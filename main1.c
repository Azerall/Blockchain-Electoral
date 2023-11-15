#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "part1.h"

int main() {

    clock_t temps_initial;
	clock_t temps_final;
	double temps_cpu;

    // Temps de calcul du test naif d'un nombre premier
    /*
    long i=3;
    temps_cpu=0;
	while(temps_cpu < 0.002) {
    	temps_initial = clock();
    	is_prime_naive(i);
    	temps_final = clock();
    	temps_cpu = ((double)(temps_final-temps_initial))/CLOCKS_PER_SEC;
        i++;
    }
	printf("Nombre premier en moins de 2 millièmes de seconde : %ld en %f secondes\n", i, temps_cpu);
    */

    // Comparaison des performances des deux méthodes d'exponentiation modulaire
    /*
    FILE *f = fopen("compare_modpow.txt", "w");
	for (int m=0; m<=500000; m+=10000) {
		temps_initial = clock();
        for (int i=0; i<100; i++) {
		    modpow_naive(4,m,497);
        }
		temps_final = clock();
		temps_cpu = ((double)(temps_final-temps_initial))/CLOCKS_PER_SEC;
		fprintf(f, "%d %f ", m, temps_cpu);

		temps_initial = clock();
        for (int i=0; i<100; i++) {
		    modpowrec(4,m,497);
        }
		temps_final = clock();
		temps_cpu = ((double)(temps_final-temps_initial))/CLOCKS_PER_SEC;
		fprintf(f, "%f\n", temps_cpu);
	}
	fclose(f);
    */

    // Tests pour vérifier le bon fonctionnement du programme
    srand(time(NULL));

    //Generation de cle :
    long p = random_prime_number(3,7,5000);
    long q = random_prime_number(3,7,5000);
    while (p == q) {
        q = random_prime_number(3,7,5000);
    }
    long n, s, u;
    generate_key_values(p, q, &n, &s, &u);
    //Pour avoir des cles positives :
    if (u<0) {
        long t = (p-1)*(q-1);
        u = u+t; //on aura toujours s*u mod t = 1
    }

    //Afichage des cles en hexadecimal
    printf("cle publique = (%lx, %lx)\n", s, n);
    printf("cle privee = (%lx, %lx)\n", u, n);

    //Chiffrement:
    char mess[10] = "Hello";
    int len = strlen(mess);
    long* crypted = encrypt(mess, s, n);

    printf("Initial message : %s \n", mess);
    printf("Encoded representation : \n");
    print_long_vector(crypted, len);

    //Dechiffrement
    char* decoded = decrypt(crypted, len, u, n);
    printf("Decoded : %s\n", decoded);

    free(crypted);
    free(decoded);

    return 0;
}
