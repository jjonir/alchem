#ifndef _ALCHEM_H
#define _ALCHEM_H

#ifdef ENABLE_LOG
#include <stdio.h>
extern FILE *logfile;
#define logf(FMT, ...) \
do { \
	fprintf(logfile, FMT, __VA_ARGS__); \
	fflush(logfile); \
} while (0)
#define log(STR) \
do { \
	fprintf(logfile, STR); \
	fflush(logfile); \
} while (0)
#endif

#endif
