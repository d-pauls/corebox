#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command.h"
static char optstring[] = "c:n:qvz";

#ifdef _GNU_SOURCE
	#include <getopt.h>
static struct option longopts[] = {
    {"bytes", required_argument, NULL, 'c'},
    {"lines", required_argument, NULL, 'n'},
    {"quiet", no_argument, NULL, 'q'},
    {"silent", no_argument, NULL, 'q'},
    {"verbose", no_argument, NULL, 'v'},
    {"zero-terminated", no_argument, NULL, 'z'},
    {NULL, 0, NULL, 0},
};
	#define GET_OPT(argc, argv, optstring) \
		getopt_long(argc, argv, optstring, longopts, NULL)
#else
	#define GET_OPT(argc, argv, optstring) getopt(argc, argv, optstring)
#endif

COMMAND(head, int argc, char *argv[]) {
	int i, num, size, exit_code;
	char line_delimeter;
	bool silent, bytes;

	silent = false;
	bytes = false;
	line_delimeter = '\n';
	num = 10;
	exit_code = 0;
	optind = 0; /* extern */

	while ((i = GET_OPT(argc, argv, optstring)) != -1) {
		switch (i) {
			case 'n':
				bytes = false;
				num = atoll(optarg);
				break;
			case 'c':
				bytes = true;
				num = atoll(optarg);
				break;
			case 'q': silent = true; break;
			case 'v': silent = false; break;
			case 'z': line_delimeter = '\0'; break;
			default: return 1;
		}
	}

	i = optind;

	if (!(size = argc - i)) {
		i = 0;
		argc = 1;
	}

	for (; i < argc; ++i) {
		FILE *file;
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

		if (!silent && size > 1) {
			char *str, *fmt;
			static int offset = 1;

			str = (file == stdin) ? "Standard Input" : argv[i];
			fmt = "\n==> %s <==\n";

			printf(fmt + offset, str);
			offset = 0;
		}

		int j = 0;
		while (j < num && (c = fgetc(file)) != EOF) {
			if (bytes || c == line_delimeter)
				++j;
			putc(c, stdout);
		}

		if (errno) {
			fprintf(
			    stderr, "%s: %s: %s\n", argv[0], argv[i],
			    strerror(errno));
			exit_code = 1;
			errno = 0;
		}

		if (file != stdin)
			fclose(file);
	}
	return exit_code;
}
