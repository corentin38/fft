/**
 * @autor <corentin@marciau.fr>
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>

#include "main.h"
#include "fft.h"
#include "cos.h"

struct fft_engine {
	FILE *signal_input;
	int sample_amount;
	int sample_amount_pow2;
	short *signal_buffer;
	double *freq_buffer;
	algorithm algo;
};

struct fft_sig_slicer {
	short *head;
	short offset;
	short step; // step 0 = slice of original size. Step++ => size /= 2
};

struct fft_freq_slicer {
	double *head;
	short offset;
	short step; // step 0 = slice of original size. Step++ => size /= 2
};

double pi;

fft_engine_t fft_engine_create (FILE* signal_input, int sample_amount, algorithm algo) {
	fft_engine_t ret = (fft_engine_t) malloc (sizeof(fft_engine_s));
	ret->signal_input = signal_input;
	ret->sample_amount = sample_amount;
	ret->sample_amount_pow2 = log2 (sample_amount);
	ret->signal_buffer = (short *) malloc (sample_amount * sizeof(short));
	ret->freq_buffer = (double *) malloc (sample_amount * sizeof(double));
	ret->algo = algo;
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

short signal_at (s_slicer sl, int i) {
	return *((sl.head + sl.offset) + (1 << sl.step) * i);
}

void set_signal_at (s_slicer sl, int i, short val) {
	*((sl.head + sl.offset) + (1 << sl.step) * i) = val;
}

short freq_at (f_slicer sl, int i) {
	return *((sl.head + sl.offset) + (1 << sl.step) * i);
}

void set_freq_at (f_slicer sl, int i, short val) {
	*((sl.head + sl.offset) + (1 << sl.step) * i) = val;
}

void fft_compute_4 (fft_engine_t self, s_slicer ssl, f_slicer fsl) {
	short x0 = signal_at (ssl, 0);
	short x1 = signal_at (ssl, 1);
	short x2 = signal_at (ssl, 2);
	short x3 = signal_at (ssl, 3);

	double X0 = x0 + x1 + x2 + x3;
	double X1 = x0 - x2;
	double X2 = x0 - x1 + x2 - x3;
	double X3 = x0 - x2;

	set_freq_at(fsl, 0, X0);
	set_freq_at(fsl, 1, X1);
	set_freq_at(fsl, 2, X2);
	set_freq_at(fsl, 3, X3);
}

double hack_cos (int numerateur, int denominateur) {
	return cos_step[ ( 1024 / denominateur ) * numerateur ];
}

void fft_add_sub_fft (fft_engine_t self, f_slicer even_sl, f_slicer odd_sl) {
	static double tmp[1024];

	if (even_sl.step != odd_sl.step) {
		fprintf(stderr,
		        "Trying to add two sub fft of different sizes : %d, %d\n",
		        even_sl.step,
		        odd_sl.step);
		return;
	}

	int size  = 1 << (10 - even_sl.step); // Number of samples of the slice
	int double_size = size * 2;

	for (int i=0; i<size; i++) {
		double even = freq_at(even_sl, i);
		double odd =  freq_at(odd_sl,  i);

		tmp[i]      = even + hack_cos (i,        double_size) * odd;
		tmp[i+size] = even + hack_cos (i + size, double_size) * odd;
	}

	for (int i=0; i<size; i++) {
		set_freq_at(even_sl, i, tmp[2*i]);
		set_freq_at(odd_sl,  i, tmp[2*i+1]);
	}
}

void fft_compute_fft (fft_engine_t self) {
	int depth = self->sample_amount_pow2;
	int size = self->sample_amount;
	int bottom = depth - 2; // Number of halvings to get to 4 elem groups

	// Computing all 4 element groups in the freq buffer
	int group4_amount = size / 4;
	for (int i=0; i<group4_amount; i++) {
		s_slicer sig_sl = {
			self->signal_buffer + i,
			i,
			bottom
		};
		f_slicer freq_sl = {
			self->freq_buffer + i,
			i,
			bottom
		};

		fft_compute_4 (self, sig_sl, freq_sl);
	}

	int step = bottom;
	while (step > 0) {
		int nb_paire_subfft = 1 << (step - 1);
		for (int i=0; i<nb_paire_subfft; i++) {
			f_slicer even = { self->freq_buffer + 2*i,     2*i, step };
			f_slicer odd  = { self->freq_buffer + 2*i + 1, 2*i, step };
			fft_add_sub_fft (self, even, odd);
		}
		step--;
	}
}

void fft_compute (fft_engine_t self) {
	pi = acos(-1.0);

	switch (self->algo) {
	case BRUTE:
		fft_compute_brute (self);
		break;
	case FFT:
		fft_compute_fft (self);
		break;
	default:
		fprintf(stderr, "%s: Unknown algorithm id : %d\n",
				__progname, self->algo);
		break;
	}
}

void fft_engine_destroy (fft_engine_t self) {
	fclose (self->signal_input);
	free (self->signal_buffer);
	free (self->freq_buffer);
	free (self);
}
