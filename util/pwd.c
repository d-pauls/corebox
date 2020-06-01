#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "command.h"

static char optstring[] = "LP";

#ifdef _GNU_SOURCE
	#include <getopt.h>
static struct option longopts[] = {
    {"logical", no_argument, NULL, 'L'},
    {"physical", no_argument, NULL, 'P'},
    {NULL, 0, NULL, 0},
};
	#define GET_OPT(argc, argv, optstring) \
		getopt_long(argc, argv, optstring, longopts, NULL)
#else
	#define GET_OPT(argc, argv, optstring) getopt(argc, argv, optstring)
#endif

COMMAND(pwd, int argc, char *argv[]) {
	struct stat a, b;
	int size, c;
	char *wd, *tmp;
	bool logical;

	logical = false;
	size = PATH_MAX;
	wd = NULL;

	while ((c = GET_OPT(argc, argv, optstring)) != -1) {
		switch (c) {
			case 'L': logical = true; break;
			case 'P': logical = false; break;
			default: return 1;
		}
	}

	while (!wd || !getcwd(wd, size)) {
		if ((errno && errno != ERANGE) ||
		    !(wd = realloc(wd, size *= 2))) {
			fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
			return 1;
		}
	}

	if (logical && (tmp = getenv("PWD")) && !stat(wd, &a) &&
	    !stat(tmp, &b) && a.st_dev == b.st_dev && a.st_ino == b.st_ino) {
		puts(tmp);
	} else {
		puts(wd);
	}

	free(wd);

	return 0;
}
