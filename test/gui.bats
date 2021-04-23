#!/usr/bin/env bats

# gui.bats requires bats imagemagick and xdotool to work properly

CUBEGL_BIN_DIR=${BATS_TEST_DIRNAME}/../../cubegl-bin
CUBEGL_BIN=cubegl
CUBEGL_WINDOW_NAME="cubegl"
CUBEGL_ABOUT_WINDOW_NAME="About Cubegl"
CUBEGL_CUBEWIDGET_REGION="466x414+165+33"

SCREENSHOT_1="${BATS_TMPDIR}/screenshot1.png"
SCREENSHOT_2="${BATS_TMPDIR}/screenshot2.png"

setup() {
	pushd ${CUBEGL_BIN_DIR}
	./${CUBEGL_BIN} 2>&1 >/dev/null &
	CUBEGL_PID=$!
	popd
	sleep 0.100
	CUBEGL_WINDOW=$(xdotool search --sync --name "^${CUBEGL_WINDOW_NAME}$")
}

teardown() {
	kill -9 ${CUBEGL_PID}
}

# Helpers

capture_two_screenshots() {
	import -window "${CUBEGL_WINDOW_NAME}" "${SCREENSHOT_1}"
	sleep 0.250s
	import -window "${CUBEGL_WINDOW_NAME}" "${SCREENSHOT_2}"
}

ignore_window_region() {
	convert "${SCREENSHOT_1}" -region $1 -fill black -colorize 100% "${SCREENSHOT_1}"
	convert "${SCREENSHOT_2}" -region $1 -fill black -colorize 100% "${SCREENSHOT_2}"
}

pictures_are_identical() {
	FIRST_ID=$(identify -quiet -format "%#" "$1")
	SECOND_ID=$(identify -quiet -format "%#" "$2")
	[ "${FIRST_ID}" = "${SECOND_ID}" ]
}

# Tests

@test "Cubegl opens a window" {
	[ 1 -eq $(xdotool search --name "^${CUBEGL_WINDOW_NAME}$" | wc -l) ]
}

@test "Cubegl window is 640x480" {
	eval $(xdotool getwindowgeometry --shell "${CUBEGL_WINDOW}")

	[ $WIDTH -eq 640 ]
	[ $HEIGHT -eq 480 ]
}

@test "Cubegl screenshots at brief interval differ only in the cube widget region" {
	capture_two_screenshots
	ignore_window_region "${CUBEGL_CUBEWIDGET_REGION}"
	pictures_are_identical "${SCREENSHOT_1}" "${SCREENSHOT_2}"
}

@test "When stop button clicked then cubegl screenshots at brief interval are identical" {
	xdotool mousemove --window "${CUBEGL_WINDOW}" 75 46 click 1
	capture_two_screenshots
	pictures_are_identical "${SCREENSHOT_1}" "${SCREENSHOT_2}"
}

@test "When clicking on File->About then about window shows up" {
	xdotool mousemove --window "${CUBEGL_WINDOW}" 30 15 click 1
	xdotool mousemove --window "${CUBEGL_WINDOW}" 30 35 click 1
	sleep 0.100
	[ 1 -eq $(xdotool search --name "^${CUBEGL_ABOUT_WINDOW_NAME}$" | wc -l) ]
}
