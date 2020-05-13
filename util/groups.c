#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command.h"

COMMAND(groups, int argc, char *argv[]) {
	int cap, ret;
	gid_t *groups;
	struct passwd *pass;
	struct group *grp;

	ret = 0;

	for (int i = 0; i < argc; ++i) {
		if (i)
			pass = getpwnam(argv[i]);
		else if (argc == 1)
			pass = getpwuid(geteuid());
		else
			continue;

		if (!pass) {
			switch (errno) {
				case 0:
				case ENOENT:
				case ESRCH:
				case EBADF:
				case EPERM:
					fprintf(
					    stderr,
					    "%s: name or uid not "
					    "found\n",
					    argv[0]);
					break;
				default:
					fprintf(
					    stderr, "%s: %s\n", argv[0],
					    strerror(errno));
					break;
			}
			ret = 1;
			continue;
		}

		cap = 0;
		groups = NULL;

		if (-1 ==
		    getgrouplist(pass->pw_name, pass->pw_gid, groups, &cap)) {
			groups = calloc(cap, sizeof(*groups));
			getgrouplist(pass->pw_name, pass->pw_gid, groups, &cap);
		}

		if (argc > 1)
			printf("%s : ", argv[i]);

		for (int j = 0; j < cap; ++j) {
			grp = getgrgid(groups[j]);
			if (j)
				putc(' ', stdout);
			printf("%s", grp ? grp->gr_name : NULL);
		}
		putc('\n', stdout);
		free(groups);
	}
	return ret;
}
