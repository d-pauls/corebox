#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "command.h"

COMMAND(sleep, int argc, char *argv[]) {
	char *prg, *str, *end;
	unsigned int total;
	long num;
	int err;

	prg = argv[0];
	total = 0;
	errno = 0;
	err = 0;

	if (argc < 2) {
		fprintf(stderr, "%s: missing operand\n", prg);
		return 1;
	}

	for (int i = 1; i < argc; ++i) {
		str = argv[i];
		num = strtol(str, &end, 10);

		if (str == end || num < 0) {
			fprintf(
			    stderr, "%s: invalid time interval '%s'\n", prg,
			    argv[i]);
			err = 1;
		} else if (total != UINT_MAX) {
			if ((unsigned long) num >= UINT_MAX ||
			    (unsigned long) num >= (UINT_MAX - total))
				total = UINT_MAX;
			else
				total += (unsigned int) num;
		}
	}

	if (err)
		return err;
	sleep(total);
	return 0;
}
