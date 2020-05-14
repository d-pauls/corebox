#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "command.h"

COMMAND(logname, int argc, char *argv[]) {
	char *name;

	if (argc > 1) {
		fprintf(stderr, "%s: extra operand '%s'\n", argv[0], argv[1]);
		return 1;
	}

	if (!(name = getlogin())) {
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
		return 1;
	}

	puts(name);
	return 0;
}
