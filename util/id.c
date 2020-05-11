#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command.h"

static char optstring[] = "gGnruz";

#ifdef _GNU_SOURCE
	#include <getopt.h>
struct option longopts[] = {
    {"group", no_argument, NULL, 'g'}, {"groups", no_argument, NULL, 'G'},
    {"name", no_argument, NULL, 'n'},  {"real", no_argument, NULL, 'r'},
    {"user", no_argument, NULL, 'u'},  {"zero", no_argument, NULL, 'z'},
};
	#define GET_OPT(argc, argv, optstring) \
		getopt_long(argc, argv, optstring, longopts, NULL)
#else
	#define GET_OPT(argc, argv, optstring) getopt(argc, argv, optstring)
#endif

enum { GROUP = 1, GROUPS, USER };

COMMAND(id, int argc, char *argv[]) {
	char nl;
	bool use_name, use_real;
	int choice, i, size, cap;

	gid_t *groups;
	struct passwd *pass;
	struct group *grp;

	nl = '\n';
	choice = 0;
	pass = NULL;
	groups = NULL;
	use_name = false;
	use_real = false;

	while ((i = GET_OPT(argc, argv, optstring)) != -1) {
		int next = 0;
		switch (i) {
			case 'g': next = GROUP; break;
			case 'G': next = GROUPS; break;
			case 'n': use_name = true; break;
			case 'r': use_real = true; break;
			case 'u': next = USER; break;
			case 'z': nl = '\0'; break;
			default: return 1;
		}

		if (next && choice != next) {
			if (choice) {
				fprintf(
				    stderr, "%s: can only use one of -gGu\n",
				    argv[0]);
				return 1;
			}
			choice = next;
		}
	}

	if (!choice && (!nl || use_real || use_name)) {
		fprintf(
		    stderr, "%s: -zrn not allowed in default format\n",
		    argv[0]);
		return 1;
	}

	i = optind;
	size = argc - i;
	errno = 0;

	if (size > 1) {
		fprintf(stderr, "%s: extra operand '%s'\n", argv[0], argv[i]);
		return 1;
	} else if (size == 1) {
		pass = getpwnam(argv[i]);
	} else {
		pass = getpwuid(use_real ? getuid() : geteuid());
	}

	if (!pass) {
		switch (errno) {
			case 0:
			case ENOENT:
			case ESRCH:
			case EBADF:
			case EPERM:
				fprintf(
				    stderr, "%s: name or uid not found\n",
				    argv[0]);
				break;
			default:
				fprintf(
				    stderr, "%s: %s\n", argv[0],
				    strerror(errno));
				break;
		}
		return 1;
	}

	if (choice == GROUP) {
		if (use_name) {
			grp = getgrgid(pass->pw_gid);
			printf("%s", grp ? grp->gr_name : NULL);
		} else {
			printf("%u", pass->pw_gid);
		}
		putc(nl, stdout);
		return 0;
	}

	if (choice == USER) {
		if (use_name)
			printf("%s", pass->pw_name);
		else
			printf("%u", pass->pw_uid);
		putc(nl, stdout);
		return 0;
	}

	if (-1 == getgrouplist(pass->pw_name, pass->pw_gid, groups, &cap)) {
		groups = calloc(cap, sizeof(*groups));
		getgrouplist(pass->pw_name, pass->pw_gid, groups, &cap);
	}

	if (choice == GROUPS) {
		for (i = 0; i < cap; ++i) {
			if (use_name) {
				grp = getgrgid(groups[i]);
				printf("%s", grp ? grp->gr_name : NULL);
			} else {
				printf("%u", groups[i]);
			}

			putc((i + 1 == cap || !nl) ? nl : ' ', stdout);
		}
	} else {
		grp = getgrgid(pass->pw_gid);
		printf("uid=%u(%s) ", pass->pw_uid, pass->pw_name);
		printf("gid=%u(%s) ", pass->pw_gid, grp ? grp->gr_name : NULL);
		printf("groups=");
		for (i = 0; groups[i] && i < cap; ++i) {
			grp = getgrgid(groups[i]);
			printf("%u(%s)", groups[i], grp ? grp->gr_name : NULL);

			if (i + 1 < cap)
				putc(',', stdout);
		}
		putc('\n', stdout);
	}
	free(groups);
	return 0;
}
