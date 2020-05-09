#include <libgen.h>
#include <stdio.h>
#include <string.h>

#include "command.h"

/* we do not want basename to be defined */
#if defined(basename) && basename == __xpg_basename
	#define basename_func __xpg_basename
	#undef basename
#else
	#define basename_func basename
#endif

COMMAND(basename, int argc, char *argv[]) {
	char *path, *suffix;

	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage: %s string [suffix]", argv[0]);
		return 1;
	}

	path = argv[1];
	suffix = argc == 3 ? argv[2] : NULL;

	if (!path[0]) {
		putc('\n', stdout);
		return 0;
	}

	path = basename_func(path);

	/* remove the suffix */
	if (suffix) {
		size_t len, suffix_len;

		len = strlen(path);
		suffix_len = strlen(suffix);

		if (suffix_len < len) {
			size_t pos = len - suffix_len;
			if (strcmp(&path[pos], suffix) == 0)
				path[pos] = '\0';
		}
	}

	puts(path);
	return 0;
}
