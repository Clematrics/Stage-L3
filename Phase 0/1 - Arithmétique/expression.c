#include <stdio.h>

int main() {
	int i;
	scanf("%d", &i);

	// calcul de (0x5e301a8b + i) <<< 5 - (i & 0x3309)
	int t = 0x5e301a8b + i;
	int t1 = t << 5; // left logical shift
	int t2 = t >> 27; // /!\ right arithmetic shift
	t2 = t2 & ~(-1 << 27); // simulate right logical shift
	i = (t1 | t2) - (i & 0x3309);

	printf("%d\n", i);
	return 0;
}