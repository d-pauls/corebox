#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "command.h"

static char optstring[] = "amnrsvo";

#ifdef _GNU_SOURCE
	#include <getopt.h>
static struct option longopts[] = {
    {"all", no_argument, NULL, 'a'},
    {"machine", no_argument, NULL, 'm'},
    {"nodename", no_argument, NULL, 'n'},
    {"kernel-release", no_argument, NULL, 'r'},
    {"kernel-name", no_argument, NULL, 's'},
    {"kernel-version", no_argument, NULL, 'v'},
    {"operating-system", no_argument, NULL, 'o'},
    {NULL, 0, NULL, 0},
};
	#define GET_OPT(argc, argv, optstring) \
		getopt_long(argc, argv, optstring, longopts, NULL)
#else
	#define GET_OPT(argc, argv, optstring) getopt(argc, argv, optstring)
#endif

#ifndef OS_NAME
	#define OS_NAME unknown
#endif

enum UNAME {
	SYS = (1 << 0),
	NODE = (1 << 1),
	REL = (1 << 2),
	VER = (1 << 3),
	MACH = (1 << 4),
	OS = (1 << 5),
};

#define ALL (SYS | NODE | REL | VER | MACH | OS)

COMMAND(uname, int argc, char *argv[]) {
	enum UNAME bitmap;
	struct utsname name;
	int i;

	for (i = 1; i < argc; ++i) {
		if (*argv[i] != '-') {
			fprintf(
			    stderr, "%s: extra operand '%s'\n", argv[0],
			    argv[i]);
			return 1;
		}
	}

	bitmap = 0;

	while ((i = getopt(argc, argv, optstring)) != -1) {
		switch (i) {
			case 'a': bitmap = ALL; break;
			case 'm': bitmap |= MACH; break;
			case 'n': bitmap |= NODE; break;
			case 'r': bitmap |= REL; break;
			case 's': bitmap |= SYS; break;
			case 'v': bitmap |= VER; break;
			case 'o': bitmap |= OS; break;
			default: return 1;
		}
	}

	if (uname(&name) == -1) {
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
		return -1;
	}

	if (bitmap == 0)
		bitmap = SYS;

	const char *uname_stats[] = {
	    name.sysname, name.nodename, name.release,
	    name.version, name.machine,	 MACRO_STR(OS_NAME),
	};

	int max = (int) sizeof(uname_stats) / sizeof(*uname_stats);
	for (int i = 0; i < max; ++i) {
		int mask = 1 << i;
		if (bitmap & mask) {
			printf("%s", uname_stats[i]);
			if (bitmap &= ~mask)
				putc(' ', stdout);
		}
	}
	puts("");

	return 0;
}
