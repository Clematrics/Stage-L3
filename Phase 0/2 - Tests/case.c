#include <stdio.h>

int main() {
	int i;
	scanf("%d", &i);

	if (i < 0)
		printf("< 0\n");
	else if (i >= 100)
		printf(">= 100\n");
	else
		printf("0 <= i < 100\n");

	return 0;
}