#include <stdio.h>
#include <string.h>

#include "GEN.h"
#include "command.h"
#include "hash.h"

#define TARGET	"corebox"
#define TGT	"CoreBox"
#define VERSION "0.0.1"

#define SWITCH(str, hash)                     \
	case hash:                            \
		/* prevent hash collisions */ \
		if (strcmp(#str, prg) != 0)   \
			break;                \
		return CALL(str, argc, argv);

#define STRINGIFY(str, ...) #str,

static void print_help(void) {
	const char *commands[] = {HASH_LIST(STRINGIFY)};
	size_t i, max;

	printf(TGT " v" VERSION "\n\n");
	printf("Usage: " TARGET " [function [arguments]...]\n");
	printf("   or: function [arguments]...\n\n");

	if (!sizeof(commands)) {
		printf("No functions found...\n");
		return;
	}

	printf("Currently defined functions:\n\t");

	max = sizeof(commands) / sizeof(*commands);

	for (i = 0; i < max - 1; ++i)
		printf("%s, ", commands[i]);
	printf("%s\n", commands[i]);
}

int main(int argc, char *argv[]) {
	/* Invalid environment */
	if (!argc || !argv)
		return 3;

	char *prg;
	int is_first_arg = 1;

	/* Check if the first argument is TARGET */
	prg = strrchr(*argv, '/');
	if (strcmp(TARGET, prg ? ++prg : *argv) == 0) {
		++argv;
		--argc;

		is_first_arg = 0;

		if (argc == 0) {
			print_help();
			return 1;
		}
	}

	if (is_first_arg && (prg = strrchr(*argv, '/')))
		++prg;
	else
		prg = *argv;

	switch (hash(prg)) {
		HASH_LIST(SWITCH)
		default: break;
	}

	fprintf(stderr, "%s: unknown first argument '%s'\n", TARGET, *argv);
	return 1;
}
