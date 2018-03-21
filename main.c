/**
 * @autor <corentin@marciau.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <getopt.h>

#include "main.h"
#include "util.h"
#include "fft.h"

#define NSAMPLES     1024
#define MAX_NSAMPLES 4096

static void usage (void);

extern char *optarg;

int main (int argc, char *argv[]) {
	__progname = argv[0];

	int   ch       = 0;
	char *s        = NULL;
	int   nsamples = NSAMPLES;
	FILE *input    = stdin;

	while ((ch = getopt(argc, argv, "s:")) != -1) {
		switch (ch) {
		case 's':
			s = optarg;
			break;
		default:
			usage();
		}
	}

	argc -= optind;

	if (argc > 1) {
		warnx("too many arguments");
	}

	if (s && sscanf(s, "%d", &nsamples) != 1)
		warnx("-s argument: not an integer");

	if (nsamples < 4 || nsamples > MAX_NSAMPLES || !ispow2(nsamples)) {
		fprintf(stderr, "%s: -s argument: should be a power of two between 4 and %d\n",
			__progname, MAX_NSAMPLES);
		nsamples = NSAMPLES;
	}

	if (argc == 1 && (input = fopen (argv[optind], "r")) == NULL) {
		fprintf(stderr, "%s: input argument should be a filename\n", __progname);
		input = stdin;
	}

	fft_engine_t instance = fft_engine_create (input, nsamples);
	fft_read_signal (instance);
	fft_compute (instance, BRUTE);
	fft_engine_destroy (instance);

	return EXIT_SUCCESS;
}

static void usage (void) {
	fprintf(stderr, "usage: %s [-s samples]\n", __progname);
	exit(1);
}
