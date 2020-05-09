#include <stdio.h>

#include "command.h"

COMMAND(yes, int argc, char *argv[]) {
	if (argc < 2) {
		while (1)
			puts("y");
	} else {
		while (1) {
			int i;
			for (i = 1; i < argc - 1; ++i)
				printf("%s ", argv[i]);
			puts(argv[i]);
		}
	}
	return 0;
}
