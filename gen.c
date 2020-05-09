#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

#define MAX_COMMANDS 256

static int add_to_list(unsigned long hash_val) {
	static unsigned long hash_list[MAX_COMMANDS];
	static size_t hash_index = 0;

	if (hash_index == MAX_COMMANDS) {
		fprintf(stderr, "Unsufficient space: ");
		fprintf(stderr, "try increasing MAX_COMMANDS\n");
		exit(1);
	}

	for (size_t i = 0; i < hash_index; i++) {
		if (hash_val == hash_list[i])
			return 1;
	}
	hash_list[hash_index++] = hash_val;
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <commands>\n", argv[0]);
		return 1;
	}

	puts("#pragma once");
	puts("#include \"command.h\"");
	printf("#define HASH_LIST(X) \t\\\n");

	for (int i = 1; i < argc; ++i) {
		unsigned long hash_val = hash(argv[i]);
		if (add_to_list(hash_val)) {
			fprintf(
			    stderr, "\nHash Collision at '%s' (hash: %lu)\n\n",
			    argv[i], hash_val);
			fprintf(stderr, "Maybe it is already defined,\n");
			fprintf(
			    stderr,
			    "otherwise try using a different hash...\n");
			return 1;
		}
		printf("\tX(%s, %lu)\t\\\n", argv[i], hash_val);
	}

	puts("\nHASH_LIST(DECLARE)");
}
