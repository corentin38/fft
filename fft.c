/**
 * @autor <corentin@marciau.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#include "main.h"
#include "fft.h"
#include "logger.h"
#include "util.h"

struct fft_engine {
	int segment_size;
	int16_t *signal_buffer;
	double *freq_re_buffer;
	double *freq_im_buffer;
};

double pi;

fft_engine_t fft_engine_create (int segment_size) {
	fft_engine_t ret = (fft_engine_t) malloc (sizeof(fft_engine_s));
	ret->segment_size = segment_size;
	ret->signal_buffer = (int16_t *) malloc (segment_size * sizeof(int16_t));
	ret->freq_re_buffer = (double *) malloc (segment_size * sizeof(double));
	ret->freq_im_buffer = NULL;

	pi = acos(-1.0);
	return ret;
}

int fft_load_segment (fft_engine_t self, int number_of_samples, const int16_t *buffer) {
	if (number_of_samples > self->segment_size) {
		loge ("Too many samples <%d> to fit segment size <%d>\n", number_of_samples, self->segment_size);
		return 1;
	}

	memcpy (self->signal_buffer, buffer, number_of_samples * sizeof(int16_t));

	if (number_of_samples < self->segment_size) {
		logm ("Too few samples <%d> to fill segment size <%d>, padding with 0.0f\n", number_of_samples, self->segment_size);
		memset (self->signal_buffer + number_of_samples, 0x0000, self->segment_size - number_of_samples);
	}

	return 0;
}

void fft_compute_brute (fft_engine_t self) {
	double N = (double) self->segment_size;

	for (int k=0; k<self->segment_size; k++) {
		double X_k = 0.0f;

		// X_k = Somme[n=0..N-1] x_n * e^-i.2.pi.k.n/N
		//     = Somme[n=0..N-1] x_n * ( cos(2 * pi * k * n / N) + i * sin(2* pi * k * n / N) )
		// We skip the imaginary part

		for (int n=0; n<self->segment_size; n++) {
			double x_n = (double) self->signal_buffer[n];
			X_k += x_n * cos (2.0f * pi * k * ( n / N ));
		}

		self->freq_re_buffer[k] = X_k;
	}
}

void
fft_compute_fft_recursive (fft_engine_t self,
                           int signal_offset,
                           int freq_offset,
                           int stride,
                           int size) {
	if (size == 1) {
		self->freq_re_buffer[freq_offset] = self->signal_buffer[signal_offset];
		self->freq_im_buffer[freq_offset] = 0;
		return;
	}

	int half_size = size >> 1; /* Because size has to be a power of 2 */

	fft_compute_fft_recursive (self, signal_offset,          freq_offset,             2 * stride, half_size);
	fft_compute_fft_recursive (self, signal_offset + stride, freq_offset + half_size, 2 * stride, half_size);

	for (int k=0; k<half_size; k++) {
		double omega_re = cos (-2.0f * pi * k * ( 1.0f / size ) );
		double omega_im = sin (-2.0f * pi * k * ( 1.0f / size ) );

		double Even_k_re = self->freq_re_buffer[freq_offset + k];
		double Even_k_im = self->freq_im_buffer[freq_offset + k];

		double Odd_k_re  = self->freq_re_buffer[freq_offset + half_size + k];
		double Odd_k_im  = self->freq_im_buffer[freq_offset + half_size + k];

		self->freq_re_buffer[freq_offset + k]             = Even_k_re + ( omega_re * Odd_k_re - omega_im * Odd_k_im );
		self->freq_im_buffer[freq_offset + k]             = Even_k_im + ( omega_re * Odd_k_im + omega_im * Odd_k_re );
		self->freq_re_buffer[freq_offset + half_size + k] = Even_k_re - ( omega_re * Odd_k_re - omega_im * Odd_k_im );
		self->freq_im_buffer[freq_offset + half_size + k] = Even_k_im - ( omega_re * Odd_k_im + omega_im * Odd_k_re );
	}
}

void fft_compute_fft (fft_engine_t self) {
	self->freq_im_buffer = (double *) malloc (self->segment_size * sizeof(double));
	fft_compute_fft_recursive (self, 0, 0, 1, self->segment_size);
}

int fft_compute (fft_engine_t self, enum algorithm_e algo) {
	switch (algo) {
	case BRUTE:
		fft_compute_brute (self);
		return 0;
	case FFT:
		fft_compute_fft (self);
		return 0;
	default:
		fprintf(stderr, "%s: Unknown algorithm id : %d\n",
				__progname, algo);
		return 1;
	}
}

int fft_write_spectrum (fft_engine_t self, char *filename, int raw) {
	FILE *output_file = fopen (filename, "w");
	char *separator = "\n";

	if (output_file == NULL) {
		loge ("Unable to write open output file: <%s>\n", filename);
		return 1;
	}

	if (raw) {
		for (int k=0; k<self->segment_size; k++) {
			fprintf (output_file, "%f%s", self->freq_re_buffer[k], separator);
		}
	} else {
		for (int k=0; k<self->segment_size/2; k++) {
			double freq = 44100.0f * ( k / ( self->segment_size * 1.0f ) );
			fprintf (output_file, "%f\t%f%s", freq, fabs (self->freq_re_buffer[k]), separator);
		}
	}

	fclose (output_file);

	return 0;
}

void
fft_print_segment_info (fft_engine_t self)
{
	char notes[12][3] = {
		"A \0",
		"A#\0",
		"B \0",
		"C \0",
		"C#\0",
		"D \0",
		"D#\0",
		"E \0",
		"F \0",
		"F#\0",
		"G \0",
		"G#\0"
	};

	double A_freq = 440.0f;

	/* Drop everything below 100Hz because my audio setup is garbage */
	double freq_step = 44100.0f / ( self->segment_size * 1.0f );
	int start_index = 0;
	double freq_index = 0.0f;
	while (freq_index < 100) {
		start_index++;
		freq_index += freq_step;
	}

	/* Find max freq */
	double peak_magnitude = 0.0f;
	int peak_index = 0;
	for (int k=start_index; k<self->segment_size/2; k++) {
		double current_magnitude = self->freq_re_buffer[k];
		if (current_magnitude > peak_magnitude) {
			peak_index = k;
			peak_magnitude = current_magnitude;
		}
	}

	double peak_freq = peak_index * freq_step;

	double lower_A = 55.0f;
	double closest_note = lower_A;
	int note_steps = 0;
	double step_factor = pow (2, 1.0f / 12.0f);
	while (closest_note < peak_freq) {
		closest_note *= step_factor;
		note_steps++;
	}
	double next_note = closest_note;
	double previous_note = closest_note /= step_factor;
	if (peak_freq - previous_note < next_note - peak_freq) {
		closest_note = previous_note;
	}

	char *found_note = notes[note_steps % 12];

	printf ("Segment frequency peak: <%f Hz> corresponding to: %s\n", peak_freq, found_note);
}

void fft_engine_destroy (fft_engine_t self) {
	free (self->signal_buffer);
	free (self->freq_re_buffer);
	if (self->freq_im_buffer != NULL) {
		free (self->freq_im_buffer);
	}
	free (self);
}
