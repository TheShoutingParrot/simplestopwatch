#include "simplestopwatch.h"
#include "config.h"

void info(const char *fname, const size_t l, const char *msg, ...) {
	va_list vargs;

	va_start(vargs, msg);
	
	fprintf(stdout, "info from file %s on line %ld: ", fname, l);
	vfprintf(stdout, msg, vargs);

	fputc('\n', stdout);

	va_end(vargs);
}

void verbose_info(const char *fname, const size_t l, const char *msg, ...) {
	if(verbosity == true) {
		va_list vargs;

		va_start(vargs, msg);
	
		fprintf(stdout, "(verbose) info from file %s on line %ld: ", fname, l);
		vfprintf(stdout, msg, vargs);

		fputc('\n', stdout);

		va_end(vargs);
	}
}

void warning(const char *fname, const size_t l, const char *msg, ...) {
	va_list vargs;

	va_start(vargs, msg);
	
	fprintf(stderr, "warning from file %s on line %ld: ", fname, l);
	vfprintf(stderr, msg, vargs);

	fputc('\n', stderr);

	va_end(vargs);
}

void die(const char *fname, const size_t l, const char *msg, ...) {
	va_list vargs;

	closeSdl();

	va_start(vargs, msg);
	
	fprintf(stderr, "error from file %s on line %ld: ", fname, l);
	vfprintf(stderr, msg, vargs);

	fputc('\n', stderr);

	va_end(vargs);

	exit(EXIT_FAILURE);
}

void usage(void) {
	fputs("usage: simpletimer [-v/-verbose] [-d/-dark] [-w/-width width] [-h/-height height]\n", stderr);
	exit(EXIT_FAILURE);
}
