/**
 * @autor <corentin@marciau.fr>
 */

#ifndef __FFT_H__
#define __FFT_H__

#include <stdint.h>

typedef struct fft_engine fft_engine_s;
typedef fft_engine_s* fft_engine_t;

enum algorithm_e {
	BRUTE,
	FFT
};

fft_engine_t fft_engine_create (int segment_size);

int fft_load_segment (fft_engine_t self, int number_of_samples, const int16_t *buffer);
int fft_compute (fft_engine_t self, enum algorithm_e algo);
void fft_engine_destroy (fft_engine_t self);

#endif /* __FFT_H__ */
