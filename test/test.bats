#!/usr/bin/env bats

WAVFFT_BIN_DIR=${BATS_TEST_DIRNAME}/..
WAVFFT_BIN=${WAVFFT_BIN_DIR}/fft
SAMPLE_DIR=${BATS_TEST_DIRNAME}/../sample
WAVE_FILE=${SAMPLE_DIR}/foo.wav

# Tests

@test "Wavfft reads file and exits" {
	${WAVFFT_BIN} --file ${WAVE_FILE}
}
