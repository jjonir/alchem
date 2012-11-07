#ifndef _ALCHEM_H
#define _ALCHEM_H

#ifdef ENABLE_LOG
#include <stdio.h>
extern FILE *logfile;
extern int indent_level;
static inline void log_print_indent(void)
{
	int i;
	for (i = 0; i < indent_level; i++)
		fprintf(logfile, "\t");
}
#define logf(FMT, ...) \
do { \
	log_print_indent(); \
	fprintf(logfile, FMT, __VA_ARGS__); \
	fflush(logfile); \
} while (0)
#define log(STR) \
do { \
	log_print_indent(); \
	fprintf(logfile, STR); \
	fflush(logfile); \
} while (0)
#endif

#define log_indent() indent_level++
#define log_unindent() indent_level--

#endif
