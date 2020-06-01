#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "command.h"

static char optstring[] = "uetAnbTEsv";

#ifdef _GNU_SOURCE
	#include <getopt.h>
static struct option longopts[] = {
    {"show-all", no_argument, NULL, 'A'},
    {"number", no_argument, NULL, 'n'},
    {"number-nonblank", no_argument, NULL, 'b'},
    {"show-tabs", no_argument, NULL, 'T'},
    {"show-ends", no_argument, NULL, 'E'},
    {"squeeze-blank", no_argument, NULL, 's'},
    {"show-nonprinting", no_argument, NULL, 'v'},
    {NULL, 0, NULL, 0},
};
	#define GET_OPT(argc, argv, optstring) \
		getopt_long(argc, argv, optstring, longopts, NULL)
#else
	#define GET_OPT(argc, argv, optstring) getopt(argc, argv, optstring)
#endif

enum { NUMBER = 1, NON_BLANK = 2 };

static void translate_print(int num) {
	int chr = num & 0xff;

	if (chr >= 0x80) {
		fputs("M-", stdout);
		chr -= 0x80;
	}

	if (chr < 0x20 || chr == 0x7f) {
		if (num >= 0x80 || (chr != '\t' && chr != '\n')) {
			putc('^', stdout);
			if (chr == 0x7f) {
				chr = 0x3f;
			} else {
				chr += 0x40;
			}
		}
	}

	putc(chr, stdout);
}

COMMAND(cat, int argc, char *argv[]) {
	FILE *file;
	int i, numbered, line, last;
	bool buffered, show_tabs, show_ends, squeeze, empty_nl, translate;

	buffered = true;
	show_tabs = false;
	show_ends = false;
	squeeze = false;
	translate = false;
	numbered = 0;
	optind = 0; /* extern */

	line = 0;
	empty_nl = false;
	last = '\n';

	while ((i = GET_OPT(argc, argv, optstring)) != -1) {
		switch (i) {
			case 'u': buffered = false; break;
			case 'n':
				if (!numbered)
					numbered = NUMBER;
				break;
			case 'b': numbered = NON_BLANK; break;
			case 'A': show_ends = true; /* fall through */
			case 't': translate = true; /* fall through */
			case 'T': show_tabs = true; break;
			case 'e': translate = true; /* fall through */
			case 'E': show_ends = true; break;
			case 's': squeeze = true; break;
			case 'v': translate = true; break;
			default: return 1;
		}
	}

	if (!buffered && setvbuf(stdout, NULL, _IONBF, 0))
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));

	i = optind;
	if (i == argc) {
		i = 0;
		argc = 1;
	}

	for (; i < argc; ++i) {
		int c;

		if (i == 0 || strcmp("-", argv[i]) == 0)
			file = stdin;
		else
			file = fopen(argv[i], "r");

		if (!file) {
			fprintf(
			    stderr, "%s: %s: %s\n", argv[0], argv[i],
			    strerror(errno));
			continue;
		}

		if (!buffered && setvbuf(file, NULL, _IONBF, 0))
			fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));

		for (; (c = fgetc(file)) != EOF; last = c) {
			bool nl = c == '\n';
			if (last == '\n') {
				if (squeeze && nl) {
					if (empty_nl)
						continue;
					else
						empty_nl = true;
				} else {
					empty_nl = false;
				}

				if (numbered && (numbered == NUMBER || !nl))
					printf("%6d\t", ++line);
			}

			if (c == '\t' && show_tabs) {
				fputs("^I", stdout);
			} else {
				if (nl && show_ends)
					putc('$', stdout);

				if (translate)
					translate_print(c);
				else
					putc(c, stdout);
			}
		}

		if (file != stdin)
			fclose(file);
	}
	return 0;
}
