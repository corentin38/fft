/**
 * @autor <corentin@marciau.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "fft.h"

struct fft_engine {
	FILE *signal_input;
	int sample_amount;
	short *signal_buffer;
	double *freq_buffer;
};

double pi;

fft_engine_t fft_engine_create (FILE* signal_input, int sample_amount) {
	fft_engine_t ret = (fft_engine_t) malloc (sizeof(fft_engine_s));
	ret->signal_input = signal_input;
	ret->sample_amount = sample_amount;
	ret->signal_buffer = (short *) malloc (sample_amount * sizeof(short));
	ret->freq_buffer = (double *) malloc (sample_amount * sizeof(double));
	return ret;
}

void aff (double *tab, int size) {
	for (int i=0; i<size; i++) {
		printf ("%lf\n", tab[i]);
		//		if (i != 0 && i % 10 == 0) printf ("\n");
	}
	//	printf("\n");
}

// For now, read sample_amount samples and quit
void fft_read_signal (fft_engine_t self) {
	int count = 0;

	char    *line   = (char *) malloc (8 * sizeof(char));
	size_t   len    = 8;
	ssize_t  nbread = 0;

	while ((nbread = getline (&line, &len, self->signal_input)) != -1 &&
	       count < self->sample_amount) {
		if (sscanf (line, "%hd", &self->signal_buffer[count++]) != 1) {
			fprintf(stderr, "%s: Invalid line, unable to parse 16 bit sample : %s\n",
				__progname, line);
			self->signal_buffer[count-1] = 0;
		}
	}

	//	aff (self->signal_buffer, count);
}

void fft_compute_brute (fft_engine_t self) {
	for (int k=0; k<self->sample_amount; k++) {
		self->freq_buffer[k] = 0;

		// X_k = Somme[n=0..N-1] x_k * s^-i.2.pi.k.n/N
		//     = Somme[n=0..N-1] x_k * ( cos(2 * pi * k * n / N) + i * sin(2* pi * k * n / N) )

		for (int n=0; n<self->sample_amount; n++) {
			double ratio = ( (double) n / self->sample_amount );
			double factor = cos (2 * pi * k * ratio);
			factor = self->signal_buffer[n] * factor;
			self->freq_buffer[k] += factor;
		}
	}

	aff (self->freq_buffer, self->sample_amount);
}

void fft_compute_fft (fft_engine_t self) {

}

void fft_compute (fft_engine_t self, enum algorithm_e algo) {
	pi = acos(-1.0);

	switch (algo) {
	case BRUTE:
		fft_compute_brute (self);
		break;
	case FFT:
		fft_compute_fft (self);
		break;
	default:
		fprintf(stderr, "%s: Unknown algorithm id : %d\n",
				__progname, algo);
		break;
	}
}

void fft_engine_destroy (fft_engine_t self) {
	fclose (self->signal_input);
	free (self->signal_buffer);
	free (self->freq_buffer);
	free (self);
}
