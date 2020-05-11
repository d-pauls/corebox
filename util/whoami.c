#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "command.h"

COMMAND(whoami, int argc, char *argv[]) {
	struct passwd *pw;

	if (argc > 1) {
		fprintf(stderr, "%s: extra operand '%s'\n", argv[0], argv[1]);
		return 1;
	}

	if (!(pw = getpwuid(geteuid()))) {
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
		return 1;
	}
	puts(pw->pw_name);
	return 0;
}
