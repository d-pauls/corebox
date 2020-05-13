#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>

#include "command.h"

COMMAND(arch, int argc, char *argv[]) {
	struct utsname name;

	if (argc > 1) {
		fprintf(stderr, "%s: extra operand '%s'\n", argv[0], argv[1]);
		return 1;
	}

	if (uname(&name) == -1) {
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
		return -1;
	}

	puts(name.machine);
	return 0;
}
