/**
 * @autor <corentin@marciau.fr>
 */

#ifndef __FFT_H__
#define __FFT_H__

typedef struct fft_engine fft_engine_s;
typedef fft_engine_s* fft_engine_t;

typedef enum algorithm_e {
	UNKNOWN,
	BRUTE,
	FFT
} algorithm;

typedef struct fft_slicer slicer;

fft_engine_t fft_engine_create (FILE* signal_input, int sample_amount, algorithm algo);

/* For now, read sample_amount samples and quit */
void fft_read_signal (fft_engine_t self);

void fft_compute (fft_engine_t self);

void fft_engine_destroy (fft_engine_t self);

#endif /* __FFT_H__ */
