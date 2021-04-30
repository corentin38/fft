/**
 * @autor <corentin@marciau.fr>
 */

#include "util.h"
#include <stdio.h>

int ispow2 (int x) {
	return x!=0 && !(x & (x-1));
}

void aff (double *tab, int size) {
	printf ("----------------------------------- <%d> doubles:\n", size);
	int count = 0;
	for (int i=0; i<size; i++) {
		printf ("%lf\t", tab[i]);
		count++;
		if (count >= 8) {
			printf ("\n");
			count = 0;
		}
	}
	printf ("\n");
}

void affs (short *tab, int size) {
	int count = 0;
	printf ("----------------------------------- <%d> shorts:\n", size);
	for (int i=0; i<size; i++) {
		printf ("%d\t", tab[i]);
		count++;
		if (count >= 8) {
			printf ("\n");
			count = 0;
		}
/*		if (count > 50) {
			printf ("\n[...] (Skipping %d samples)\n", size - count);
			break;
			}*/
	}
	printf ("\n");
}
