/**
 * @autor <corentin@marciau.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>

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
	char *str_nsamples      = NULL;
	int   nsamples = NSAMPLES;
	int   custom_input_file_flag = 0;
	char *str_input      = NULL;
	FILE *input    = stdin;

	/* Options:
	 *     -i    Input file
	 *     -s    Number of samples to read in input file
	 */
	while ((ch = getopt(argc, argv, "i:s:")) != -1) {
		switch (ch) {
		case 'i':
			custom_input_file_flag = 1;
			str_input = optarg;
			break;
		case 's':
			str_nsamples = optarg;
			break;
		default:
			usage();
		}
	}

	argc -= optind;

	if (argc > 1) {
		warnx("too many arguments");
		exit (EXIT_FAILURE);
	}

	if (str_nsamples && sscanf(str_nsamples, "%d", &nsamples) != 1) {
		warnx("-s argument (sample number to read in input file) : not an integer");
		exit (EXIT_FAILURE);
	}

	if (nsamples < 4 || nsamples > MAX_NSAMPLES || !ispow2(nsamples)) {
		fprintf(stderr, "%s: -s argument (sample number to read in input file) : should be a power of two between 4 and %d\n",
			__progname, MAX_NSAMPLES);
		nsamples = NSAMPLES;
		exit (EXIT_FAILURE);
	}

	if (custom_input_file_flag) {
		if ((input = fopen (str_input, "r")) == NULL) {
			fprintf(stderr, "%s: -i argument (input file name) : should should be a valid readable filename.\n", __progname);
			exit (EXIT_FAILURE);
		}
	} else {
		input = stdin;
	}

	printf ("Starting fft engine reading <%d> samples from file <%s>.\n",
	        nsamples,
	        custom_input_file_flag ? str_input : "standard input");

	fft_engine_t instance = fft_engine_create (input, nsamples);
	fft_read_signal (instance);
	fft_compute (instance, BRUTE);
	fft_engine_destroy (instance);

	return EXIT_SUCCESS;
}

static void usage (void) {
	fprintf(stderr, "usage: %s [-i input_file] [-s number_of_samples_to_read]\n", __progname);
	exit (EXIT_FAILURE);
}
