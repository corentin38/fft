#!/usr/bin/env bats

WAVFFT_BIN_DIR=${BATS_TEST_DIRNAME}/..
WAVFFT_BIN=${WAVFFT_BIN_DIR}/fft
SAMPLE_DIR=${BATS_TEST_DIRNAME}/../sample
WAVE_FILE=${SAMPLE_DIR}/foo.wav
OUTPUT_FILE=${WAVFFT_BIN_DIR}/spectrum.out

float_equal () {
	[ $(bc <<<"$1 == $2") -eq 1 ]
}

list_equal () {
	NSAMPLES=$(echo -n "$1" | grep -c "^")
	for INDEX in $(seq ${NSAMPLES})
	do
		VAL_FIRST=$(echo "$1" | head -${INDEX} | tail -1)
		VAL_SECOND=$(echo "$2" | head -${INDEX} | tail -1)

		float_equal "${VAL_FIRST}" "${VAL_SECOND}"
	done
}

# Tests

@test "Wavfft on 1 sample gives the sample" {
	${WAVFFT_BIN} -n 1 -t BRUTE -r ${WAVE_FILE}

	OUTPUT=$(cat ${OUTPUT_FILE})
	echo "Wavfft output value: ${OUTPUT}"

	float_equal "${OUTPUT}" -3129
}

@test "Wavfft on 1 sample gives same result for BRUTE and FFT" {
	${WAVFFT_BIN} -n 1 -t BRUTE -r ${WAVE_FILE}
	OUTPUT_BRUTE=$(cat ${OUTPUT_FILE})
	echo "Wavfft BRUTE output value: ${OUTPUT_BRUTE}"

	${WAVFFT_BIN} -n 1 -t FFT -r ${WAVE_FILE}
	OUTPUT_FFT=$(cat ${OUTPUT_FILE})
	echo "Wavfft FFT output value: ${OUTPUT_FFT}"

	float_equal "${OUTPUT_BRUTE}" "${OUTPUT_FFT}"
}

@test "Wavfft on 2 sample gives same result for BRUTE and FFT" {
	${WAVFFT_BIN} -n 2 -t BRUTE -r ${WAVE_FILE}
	OUTPUT_BRUTE=$(cat ${OUTPUT_FILE})
	echo "Wavfft BRUTE output value: ${OUTPUT_BRUTE}"

	${WAVFFT_BIN} -n 2 -t FFT -r ${WAVE_FILE}
	OUTPUT_FFT=$(cat ${OUTPUT_FILE})
	echo "Wavfft FFT output value: ${OUTPUT_FFT}"

	list_equal "${OUTPUT_BRUTE}" "${OUTPUT_FFT}"
}

@test "Wavfft on 4 sample gives same result for BRUTE and FFT" {
	${WAVFFT_BIN} -n 4 -t BRUTE -r ${WAVE_FILE}
	OUTPUT_BRUTE=$(cat ${OUTPUT_FILE})
	echo "Wavfft BRUTE output value: ${OUTPUT_BRUTE}"

	${WAVFFT_BIN} -n 4 -t FFT -r ${WAVE_FILE}
	OUTPUT_FFT=$(cat ${OUTPUT_FILE})
	echo "Wavfft FFT output value: ${OUTPUT_FFT}"

	list_equal "${OUTPUT_BRUTE}" "${OUTPUT_FFT}"
}

@test "Wavfft on 8 sample gives same result for BRUTE and FFT" {
	${WAVFFT_BIN} -n 8 -t BRUTE -r ${WAVE_FILE}
	OUTPUT_BRUTE=$(cat ${OUTPUT_FILE})
	echo "Wavfft BRUTE output value: ${OUTPUT_BRUTE}"

	${WAVFFT_BIN} -n 8 -t FFT -r ${WAVE_FILE}
	OUTPUT_FFT=$(cat ${OUTPUT_FILE})
	echo "Wavfft FFT output value: ${OUTPUT_FFT}"

	list_equal "${OUTPUT_BRUTE}" "${OUTPUT_FFT}"
}

@test "Wavfft on 128 sample gives same result for BRUTE and FFT" {
	${WAVFFT_BIN} -n 128 -t BRUTE -r ${WAVE_FILE}
	OUTPUT_BRUTE=$(cat ${OUTPUT_FILE})
	echo "Wavfft BRUTE output value: ${OUTPUT_BRUTE}"

	${WAVFFT_BIN} -n 128 -t FFT -r ${WAVE_FILE}
	OUTPUT_FFT=$(cat ${OUTPUT_FILE})
	echo "Wavfft FFT output value: ${OUTPUT_FFT}"

	list_equal "${OUTPUT_BRUTE}" "${OUTPUT_FFT}"
}
