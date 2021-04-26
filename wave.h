/**
 * @autor <corentin@marciau.fr>
 */

#ifndef __WAVE_H__
#define __WAVE_H__

typedef struct wave_reader wave_reader_s;
typedef wave_reader_s* wave_reader_t;

wave_reader_t wave_reader_create (FILE *source_file, int nsamples);
int wave_read_header (wave_reader_t self);
void wave_print_description (wave_reader_t self);
int wave_print_next_samples (wave_reader_t self);
int wave_read_next_samples_first_chan (wave_reader_t self, int16_t *buffer);
void wave_reader_destroy (wave_reader_t self);

#endif /* __WAVE_H__ */
