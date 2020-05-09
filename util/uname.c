#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "command.h"

enum UNAME {
	SYS = (1 << 0),
	NODE = (1 << 1),
	REL = (1 << 2),
	VER = (1 << 3),
	MACH = (1 << 4),

#ifdef _GNU_SOURCE
	DOM = (1 << 5),
#endif

};

#ifdef _GNU_SOURCE
	#define ALL (SYS | NODE | REL | VER | MACH | DOM)
#else
	#define ALL (SYS | NODE | REL | VER | MACH)
#endif

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

	while ((i = getopt(argc, argv, "amnrsv")) != -1) {
		switch (i) {
			case 'a': bitmap = ALL; break;
			case 'm': bitmap |= MACH; break;
			case 'n': bitmap |= NODE; break;
			case 'r': bitmap |= REL; break;
			case 's': bitmap |= SYS; break;
			case 'v': bitmap |= VER; break;
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
	    name.sysname,    name.nodename, name.release,
	    name.version,    name.machine,
#ifdef _GNU_SOURCE
	    name.domainname,
#endif
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
