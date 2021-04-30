/**
 * @autor <corentin@marciau.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <string.h>
#include <math.h>

#include "main.h"
#include "util.h"
#include "fft.h"
#include "wave.h"
#include "logger.h"

#define NSAMPLES     1024
#define MAX_NSAMPLES 65536
#define MAX_POW      20

static void usage (void);
extern char *optarg;

int main (int argc, char *argv[]) {
	__progname = argv[0];

	int   ch       = 0;
	char *str_nsamples      = NULL;
	int   nsamples = NSAMPLES;
	int nsamples_as_pow = 0;
	int raw = 0;

	char *str_type = NULL;
	enum algorithm_e type = BRUTE;

	int custom_input_file_flag = 0;
	char *str_input      = NULL;
	FILE *input    = stdin;

	/* Options:
	 *     -n    Number of samples from input on which to perform transform
	 *     -p    Number of samples as a power of two
	 *     -v    Print all log messages
	 *     -t    [BRUTE,FFT] Use bruteforce algo or fft
	 *     -r    raw, write full fft result in output file
	 */
	while ((ch = getopt(argc, argv, "rvp:n:t:")) != -1) {
		switch (ch) {
		case 'n':
			str_nsamples = optarg;
			break;
		case 'p':
			str_nsamples = optarg;
			nsamples_as_pow = 1;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'r':
			raw = 1;
			break;
		case 't':
			str_type = optarg;
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

	if (argc == 1) {
		custom_input_file_flag = 1;
		str_input = argv[optind];
	}

	if (!nsamples_as_pow && str_nsamples && sscanf(str_nsamples, "%d", &nsamples) != 1) {
		warnx("-n argument (number of samples for fourier segment) : not an integer");
		exit (EXIT_FAILURE);
	}

	if (!nsamples_as_pow && (nsamples < 1 || nsamples > MAX_NSAMPLES || !ispow2(nsamples))) {
		fprintf(stderr, "%s: -n argument (number of samples for fourier segment) : should be a power of two between 1 and %d\n",
		        __progname, MAX_NSAMPLES);
		exit (EXIT_FAILURE);
	}

	if (nsamples_as_pow && str_nsamples && sscanf(str_nsamples, "%d", &nsamples) != 1) {
		warnx("-p argument (number of samples for fourier segment as a power of two) : not an integer");
		exit (EXIT_FAILURE);
	}

	if (nsamples_as_pow && nsamples > 20) {
		fprintf(stderr, "%s: -p argument (number of samples for fourier segment as a power of two) : should be lower than %d\n",
		        __progname, MAX_POW);
		exit (EXIT_FAILURE);
	}

	if (nsamples_as_pow) {
		nsamples = pow (2, nsamples);
	}

	if (str_type != NULL) {
		if (strcmp (str_type, "FFT") == 0) {
			type = FFT;
		} else if (strcmp (str_type, "BRUTE") == 0) {
			type = BRUTE;
		} else {
			loge ("Unknown type <%s>", str_type);
		}
	}

	if (custom_input_file_flag) {
		if ((input = fopen (str_input, "r")) == NULL) {
			fprintf(stderr, "%s: [FILE] argument (input file name) : should should be a valid readable filename.\n", __progname);
			exit (EXIT_FAILURE);
		}
	} else {
		input = stdin;
	}

	printf ("Starting fft engine reading <%d> samples from file <%s>.\n",
	        nsamples,
	        custom_input_file_flag ? str_input : "standard input");

	wave_reader_t reader = wave_reader_create (input, nsamples);
	if (wave_read_header (reader) != 0) {
		loge ("Bad input wave file\n");
		return EXIT_FAILURE;
	}

	fft_engine_t instance = fft_engine_create (nsamples);

	int16_t *buffer = malloc (nsamples * sizeof (int16_t));
	int read = 0;
	read = wave_read_next_samples_first_chan (reader, buffer);

	fft_load_segment (instance, read, buffer);

	fft_compute (instance, type);

	fft_write_spectrum (instance, "spectrum.out", raw);

	fft_engine_destroy (instance);

	return EXIT_SUCCESS;
}

static void usage (void) {
	fprintf(stderr, "usage: %s [-n segment_size] [WAVE_FILE]\n", __progname);
	exit (EXIT_FAILURE);
}
