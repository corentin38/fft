/**
 * @autor <corentin@marciau.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

#include "main.h"
#include "fft.h"

struct fft_engine {
	FILE *signal_input;
	int sample_amount;
	unsigned short *signal_buffer;
	double *freq_buffer;
};

double pi;

fft_engine_t fft_engine_create (FILE* signal_input, int sample_amount) {
	fft_engine_t ret = (fft_engine_t) malloc (sizeof(fft_engine_s));
	ret->signal_input = signal_input;
	ret->sample_amount = sample_amount;
	ret->signal_buffer = (unsigned short *) malloc (sample_amount * sizeof(unsigned short));
	ret->freq_buffer = (double *) malloc (sample_amount * sizeof(double));
	return ret;
}

void aff (double *tab, int size) {
	int count = 0;
	for (int i=0; i<size; i++) {
		printf ("%lf\n", tab[i]);
		count++;
		if (count > 50) {
			printf ("\n[...] (Skipping %d samples)\n", size - count);
			break;
		}
	}
}

void affs (unsigned short *tab, int size) {
	int count = 0;
	for (int i=0; i<size; i++) {
		printf ("%hu\n", tab[i]);
		count++;
		if (count > 50) {
			printf ("\n[...] (Skipping %d samples)\n", size - count);
			break;
		}
	}
}

// For now, read sample_amount samples and quit
int fft_read_signal (fft_engine_t self) {
	ssize_t  nbread = 0;
	unsigned short sample = 0;
	int fscanf_ret = 0;

	for (nbread = 0; nbread < self->sample_amount; nbread++) {
		errno = 0;

		fscanf_ret = fscanf (self->signal_input, "%hu,", &sample);

		if (fscanf_ret == 1) {
			self->signal_buffer[nbread] = sample;
		} else if (errno != 0) {
			perror ("fscanf");
			fprintf (stderr, "%s: fscanf parsing error while reading sample at position #%ld\n", __progname, nbread);
			return 1;
		} else {
			fprintf (stderr, "%s: Invalid sample at position #%ld\n", __progname, nbread);
			return 2;
		}
	}

	affs (self->signal_buffer, self->sample_amount);
	return 0;
}

int fft_compute_brute (fft_engine_t self) {
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
	return 0;
}

int fft_compute_fft (fft_engine_t self) {
	return 0;
}

int fft_compute (fft_engine_t self, enum algorithm_e algo) {
	pi = acos(-1.0);

	switch (algo) {
	case BRUTE:
		return fft_compute_brute (self);
	case FFT:
		return fft_compute_fft (self);
	default:
		fprintf(stderr, "%s: Unknown algorithm id : %d\n",
				__progname, algo);
		return 1;
	}
}

void fft_engine_destroy (fft_engine_t self) {
	fclose (self->signal_input);
	free (self->signal_buffer);
	free (self->freq_buffer);
	free (self);
}
