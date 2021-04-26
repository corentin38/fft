/**
 * @autor <corentin@marciau.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "wave.h"
#include "main.h"
#include "logger.h"

struct wave_reader {
	FILE *source_file;
	int nsamples;
	uint16_t format_code;
	char format_code_str[16];
	uint16_t n_channels;
	uint32_t n_samples_per_sec;
	uint32_t avg_bytes_per_sec;
	uint16_t block_size;
	uint16_t bits_per_sample;
	uint32_t data_chunk_size;
};

wave_reader_t
wave_reader_create (FILE *source_file,
                    int nsamples)
{
	wave_reader_t ret = (wave_reader_t) malloc (sizeof (wave_reader_s));
	ret->source_file = source_file;
	ret->nsamples = nsamples;
	ret->format_code = 0;
	strcpy (ret->format_code_str, "undefined");
	ret->n_channels = 0;
	ret->n_samples_per_sec = 0;
	ret->avg_bytes_per_sec = 0;
	ret->block_size = 0;
	ret->bits_per_sample = 0;
	ret->data_chunk_size = 0;
	return ret;
}

int
wave_read_header (wave_reader_t self)
{
	size_t nread = 0;

	/* Reading RIFF header */
	char ckID[5] = { 0, 0, 0, 0, '\0' };
	nread = fread (ckID, 1, 4, self->source_file);

	if (nread != 4) {
		loge ("unable to read 4 bytes RIFF header\n");
		return 1;
	}

	if (strcmp (ckID, "RIFF") != 0) {
		loge ("invalid wave input : wrong RIFF header <%s>\n", ckID);
		return 1;
	}

	/* Reading RIFF chunk size */
	uint32_t cksize;
	nread = fread (&cksize, 1, 4, self->source_file);

	if (nread != 4) {
		loge ("unable to read 4 bytes RIFF chunk size\n");
		return 1;
	}

	/* Reading WAVE ID */
	char WAVEID[5] = { 0, 0, 0, 0, '\0' };
	nread = fread (WAVEID, 1, 4, self->source_file);

	if (nread != 4) {
		loge ("unable to read 4 bytes WAVE ID\n");
		return 1;
	}

	if (strcmp (WAVEID, "WAVE") != 0) {
		loge ("invalid wave input : wrong WAVE ID <%s>\n", ckID);
		return 1;
	}

	logm ("Source contains a RIFF WAVE chunk of <%d> bytes\n", cksize);

	/* Reading first WAVE chunk, fmt */
	char fmtckID[5] = { 0, 0, 0, 0, '\0' };
	nread = fread (fmtckID, 1, 4, self->source_file);

	if (nread != 4) {
		loge ("unable to read 4 bytes fmt chunk ID\n");
		return 1;
	}

	if (strcmp (fmtckID, "fmt ") != 0) {
		loge ("invalid wave input : first WAVE chunk ID is not fmt: <%s>\n", fmtckID);
		return 1;
	}

	/* Reading fmt chunk size */
	uint32_t fmtcksize;
	nread = fread (&fmtcksize, 1, 4, self->source_file);

	if (nread != 4) {
		loge ("unable to read 4 bytes fmt chunk size\n");
		return 1;
	}

	if (fmtcksize != 16 && fmtcksize != 18 && fmtcksize != 40) {
		loge ("invalid fmt chunk, size is not 16, 18 or 40 bytes : <%d>\n", fmtcksize);
		return 1;
	}

	logm ("WAVE contains an fmt chunk of <%d> bytes\n", fmtcksize);

	char *fmtchunk = malloc (fmtcksize * sizeof(char));
	char *fmt_ptr = fmtchunk;
	nread = fread (fmtchunk, 1, fmtcksize, self->source_file);

	if (nread != fmtcksize) {
		loge ("unable to read fmt chunk, end of file reached\n");
		return 1;
	}

	/* Reading format code */
	memcpy (&self->format_code, fmt_ptr, 2);
	fmt_ptr += 2;

	switch (self->format_code) {
	case 0x0001:
		strcpy (self->format_code_str, "PCM");
		break;
	case 0x0003:
		strcpy (self->format_code_str, "IEEE float");
		break;
	case 0x0006:
		strcpy (self->format_code_str, "A-Law 8b G.711");
		break;
	case 0x0007:
		strcpy (self->format_code_str, "Mu-Law 8b G.711");
		break;
	case 0xFFFE:
		strcpy (self->format_code_str, "Extensible");
		break;
	default:
		strcpy(self->format_code_str, "Unknown");
	}

	/* Reading number of channels */
	memcpy (&self->n_channels, fmt_ptr, 2);
	fmt_ptr += 2;

	/* Reading sampling rate (blocks per second) */
	memcpy (&self->n_samples_per_sec, fmt_ptr, 4);
	fmt_ptr += 4;

	/* Reading data rate (bytes per second) */
	memcpy (&self->avg_bytes_per_sec, fmt_ptr, 4);
	fmt_ptr += 4;

	/* Reading block size */
	memcpy (&self->block_size, fmt_ptr, 2);
	fmt_ptr += 2;

	/* Reading bits per sample */
	memcpy (&self->bits_per_sample, fmt_ptr, 2);
	fmt_ptr += 2;

	if (self->format_code != 0x0001 /* PCM */) {
		logw ("Warning: wave reader only supports PCM format. Current format <%s> is unsupported\n", self->format_code_str);
	}

	free (fmtchunk);

	/* Reading second WAVE chunk, data */
	char datackID[5] = { 0, 0, 0, 0, '\0' };
	nread = fread (datackID, 1, 4, self->source_file);

	if (nread != 4) {
		loge ("unable to read 4 bytes data chunk ID\n");
		return 1;
	}

	if (strcmp (datackID, "data") != 0) {
		loge ("invalid wave input : second WAVE chunk ID is not data: <%s>\n", datackID);
		return 1;
	}

	/* Reading data chunk size */
	nread = fread (&self->data_chunk_size, 1, 4, self->source_file);

	if (nread != 4) {
		loge ("unable to read 4 bytes data chunk size\n");
		return 1;
	}

	logm ("Expecting <%d> bytes of audio data\n", self->data_chunk_size);
	return 0;
}

void
wave_print_description (wave_reader_t self)
{
	printf ("------------------------\n");
	printf ("WAVE Source:\n");
	printf ("\tFormat: %s\n", self->format_code_str);
	printf ("\tChannels: %d\n", self->n_channels);
	printf ("\tSampling rate: %d samples / s\n", self->n_samples_per_sec);
	printf ("\tData rate: %d bytes / s\n", self->avg_bytes_per_sec);
	printf ("\tBlock size: %d bytes\n", self->block_size);
	printf ("\tBits per sample: %d bits\n", self->bits_per_sample);
	printf ("------------------------\n");
}

int
wave_print_next_samples (wave_reader_t self)
{
	int segment_size = self->nsamples;
	int nread = 0;

	char buff[4]; /* Only supporting PCM 16 bit samples on 2 channels */
	short sample1 = 0, sample2 = 0;

	for (int i=0; i<segment_size; i++) {
		if (feof (self->source_file)) {
			logm ("Interrupting before fulle segment is read because end of file reached\n");
			return 1;
		}

		nread = fread (buff, 1, 4, self->source_file);

		if (nread != 4 && !feof (self->source_file)) {
			loge ("unable to read incomplete sample n° %d in current segment which is only %d byte long\n", nread);
			return 1;
		}

		memcpy (&sample1, buff, 2);
		memcpy (&sample2, buff+2, 2);

		printf ("channel-1: %5d        channel-2: %5d\n", sample1, sample2);
	}

	return 0;
}

int
wave_read_next_samples_first_chan (wave_reader_t self,
                                   int16_t *buffer)
{
	int segment_size = self->nsamples;
	int nread = 0;

	char buff[4]; /* Only supporting PCM 16 bit samples on 2 channels */
	short sample1 = 0, sample2 = 0;

	for (int i=0; i<segment_size; i++) {
		if (feof (self->source_file)) {
			logm ("Interrupting before fulle segment is read because end of file reached\n");
			return i;
		}

		nread = fread (buff, 1, 4, self->source_file);

		if (nread != 4 && !feof (self->source_file)) {
			loge ("unable to read incomplete sample n° %d in current segment which is only %d byte long\n", nread);
			return i;
		}

		memcpy (&sample1, buff, 2);
		memcpy (&sample2, buff+2, 2);

		buffer[i] = sample1;
	}

	return self->nsamples;
}

void
wave_reader_destroy (wave_reader_t self)
{
	if (0 != fclose (self->source_file)) {
		loge ("Unable to even close source file!\n");
	}
}
