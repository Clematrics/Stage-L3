#include <stdio.h>

int main() {
	int input;
	scanf("%d", &input);
	unsigned int i = input; // conversion en non signé pour permettre d'effectuer des right logical shifts

	// affichage du binaire de i
	unsigned int m = 1 << 31; // masque 1000 0000 ... 0000
	for (unsigned int j = 0; j < 32; j++) {
		printf(i & m ? "1" : "0");
		if ((j + 1) % 4 == 0) printf(" ");
		m = m >> 1;
	}
	printf("\n");

	// algorithme tiré de https://graphics.stanford.edu/~seander/bithacks.html#BitReverseObvious
	int r = i; // contiendra le miroir de i
	int s = 31; // nombres de bits restant à décaler
	for (i >>= 1; i; i >>= 1) {
		r <<= 1;
		r |= i & 1;
		s--;
	}
	r <<= s;

	// affichage du binaire de r
	m = 1 << 31; // masque 1000 0000 ... 0000
	for (unsigned int j = 0; j < 32; j++) {
		printf(r & m ? "1" : "0");
		if ((j + 1) % 4 == 0) printf(" ");
		m = m >> 1;
	}
	printf("\n");

	return 0;
}