#include "part1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int is_prime_naive(long p) {
  // Etant donné un entier impair p, renvoie 1 si p est premier et 0 sinon.
  if (p <= 1) {
    return 0; // 0 et 1 ne sont pas des nombres premiers
  }
  if (p <= 3) {
    return 1; // 2 et 3 sont des nombres premiers
  }
  for (long i = 3; i < p;
       i++) { // on vérifie qu'aucun des entiers entre 3 et p−1 ne divise p
    if (p % i == 0) {
      return 0;
    }
  }
  return 1;
}

long modpow_naive(long a, long m, long n) {
  // Fonction qui retourne la valeur a^m mod n par la méthode naïve
  long res = 1;
  while (m > 0) {
    res *= a;
    res %= n;
    m--;
  }
  return res;
}

long modpow(long a, long m, long n) {
  // Fonction qui retourne la valeur a^m mod n
  long res = 1;
  long tmp = a;
  while (m > 0) {
    if (m % 2 == 1) { // si m est impair
      res = (res * tmp) % n;
    }
    tmp = (tmp * tmp) % n;
    m /= 2;
  }
  return res % n;
}

long modpowrec(long a, long m, long n) {
  // Fonction récursive qui retourne la valeur a^m mod n
  if (m == 0) { // Cas de base
    return 1;
  }
  long b = modpowrec(a, m / 2, n);
  if (m % 2 == 0) { // si m est pair
    return (b * b) % n;
  }
  return (a * b * b) % n; // sinon, m est impair
}

int witness(long a, long b, long d, long p) {
  // Fonction qui qui teste si a est un témoin de Miller pour p, pour un entier
  // a donné
  long x = modpowrec(a, d, p);
  if (x == 1) {
    return 0;
  }
  for (long i = 0; i < b; i++) {
    if (x == p - 1) {
      return 0;
    }
    x = modpowrec(x, 2, p);
  }
  return 1;
}

long rand_long(long low, long up) {
  // Fonction qui retourne un entier long généré aléatoirement entre low et up
  // inclus
  return rand() % (up - low + 1) + low;
}

int is_prime_miller(long p, int k) {
  // Fonction qui réalise le test de Miller-Rabin.
  // la fonction retourne 0 si p n'est pas premier, et retourne 1 si p est
  // probablement premier.
  if (p == 2) {
    return 1;
  }
  if (!(p & 1) || p <= 1) { // on verifie que p est impair et different de 1
    return 0;
  }
  // on determine b et d :
  long b = 0;
  long d = p - 1;
  while (!(d & 1)) { // tant que d n'est pas impair
    d = d / 2;
    b = b + 1;
  }
  // On genere k valeurs pour a, et on teste si c'est un temoin :
  long a;
  int i;
  for (i = 0; i < k; i++) {
    a = rand_long(2, p - 1);
    if (witness(a, b, d, p)) {
      return 0;
    }
  }
  return 1;
}

long puissance(long a, long m) {
  // Fonction qui calcule a^m
  if (m == 1) {
    return a;
  }
  long b = puissance(a, m / 2);
  if (m % 2 == 0) {
    return b * b;
  }
  return a * b * b;
}

long random_prime_number(int low_size, int up_size, int k) {
  // Fonction qui retourne un nombre premier de taille comprise entre low size
  // et up size.
  long low = puissance(2, low_size - 1); // le plus petit entier à low_size bits
  long up = puissance(2, up_size) - 1;   // le plus petit entier à up_size bits
  long p = rand_long(low, up);
  while (!is_prime_miller(p, k)) {
    p = rand_long(low, up);
  }
  return p;
}

long extended_gcd(long s, long t, long *u, long *v) {
  // Version récursive de l'algorithme d'Euclide étendu
  if (s == 0) {
    *u = 0;
    *v = 1;
    return t;
  }
  long uPrim, vPrim;
  long gcd = extended_gcd(t % s, s, &uPrim, &vPrim);
  *u = vPrim - (t / s) * uPrim;
  *v = uPrim;
  return gcd;
}

void generate_key_values(long p, long q, long *n, long *s, long *u) {
  // permet de générer la clé publique pkey = (s, n) et la clé secrète skey =
  // (u, n), à partir des nombres premiers p et q, en suivant le protocole RSA
  *n = p * q;
  long t = (p - 1) * (q - 1);
  *s = rand_long(0, t - 1); // t n'est pas inclu
  long v = 1;
  while (extended_gcd(*s, t, u, &v) != 1) {
    *s = rand_long(0, t - 1);
  }
}

long *encrypt(char *chaine, long s, long n) {
  // chiffre la chaîne de caractères chaine avec la clé publique pKey = (s, n)
  int l = strlen(chaine);
  long *tab = malloc(sizeof(long) * l);
  if (tab == NULL) { // on vérifie que l'allocation a fonctionné
    printf("Problème lors de l'allocation");
    return NULL;
  }
  for (int i = 0; i < l; i++) {
    tab[i] = modpowrec(chaine[i], s, n);
  }
  return tab;
}

char *decrypt(long *crypted, int size, long u, long n) {
  // déchiffre un message à l’aide de la clé secrète skey = (u, n), en
  // connaissant la taille du tableau d’entiers
  char *tab = malloc(sizeof(char) * (size + 1));
  if (tab == NULL) { // on vérifie que l'allocation a fonctionné
    printf("Problème lors de l'allocation");
    return NULL;
  }
  for (int i = 0; i < size; i++) {
    tab[i] = modpowrec(crypted[i], u, n);
  }
  tab[size] = '\0';
  return tab;
}

void print_long_vector(long *result, int size) {
  printf("Vector : [");
  for (int i = 0; i < size; i++) {
    printf("%lx \t", result[i]);
  }
  printf("]\n");
}
