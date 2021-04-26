/**
 * @autor <corentin@marciau.fr>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "util.h"

int verbose = 0;

void logm (const char *fmt, ...)
{
	if (!verbose) return;

	static char format_str[1024];
	char *format_str_long = NULL;

	int fmt_length = strlen (fmt);
	if (fmt_length > 1013) {
		format_str_long = malloc ((fmt_length + 10) * sizeof (char));
		strcpy (format_str_long, "[MESSAGE] ");
		strcpy ((format_str_long + 10), fmt);
		fmt = format_str_long;
	} else {
		strcpy (format_str, "[MESSAGE] ");
		strcpy ((format_str + 10), fmt);
		fmt = format_str;
	}

	va_list ap;
	va_start (ap, fmt);
	vprintf (fmt, ap);
	va_end (ap);

	if (fmt_length > 1013) {
		free (format_str_long);
	}
}

void logw (const char *fmt, ...)
{
	static char format_str[1024];
	char *format_str_long = NULL;

	int fmt_length = strlen (fmt);
	if (fmt_length > 1013) {
		format_str_long = malloc ((fmt_length + 10) * sizeof (char));
		strcpy (format_str_long, "[WARNING] ");
		strcpy ((format_str_long + 10), fmt);
		fmt = format_str_long;
	} else {
		strcpy (format_str, "[WARNING] ");
		strcpy ((format_str + 10), fmt);
		fmt = format_str;
	}

	va_list ap;
	va_start (ap, fmt);
	vprintf (fmt, ap);
	va_end (ap);

	if (fmt_length > 1013) {
		free (format_str_long);
	}
}

void loge (const char *fmt, ...)
{
	static char format_str[1024];
	char *format_str_long = NULL;

	int fmt_length = strlen (fmt);
	if (fmt_length > 1015) {
		format_str_long = malloc ((fmt_length + 8) * sizeof (char));
		strcpy (format_str_long, "[ERROR] ");
		strcpy ((format_str_long + 8), fmt);
		fmt = format_str_long;
	} else {
		strcpy (format_str, "[ERROR] ");
		strcpy ((format_str + 8), fmt);
		fmt = format_str;
	}

	va_list ap;
	va_start (ap, fmt);
	vfprintf (stderr, fmt, ap);
	va_end (ap);

	if (fmt_length > 1015) {
		free (format_str_long);
	}
}
