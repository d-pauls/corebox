#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "command.h"

COMMAND(tty, int argc, char *argv[]) {
	char *name;

	if (argc > 1) {
		fprintf(stderr, "%s: extra operand '%s'\n", argv[0], argv[1]);
		return 1;
	}

	if (!(name = ttyname(STDIN_FILENO))) {
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
		return 1;
	}

	puts(name);
	return 0;
}
