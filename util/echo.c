#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command.h"

static int get_num(int c, bool is_hex) {
	if (c >= '0' && c <= '9')
		return c - '0';
	c = tolower(c);
	if (is_hex && c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return -1;
}

COMMAND(echo, int argc, char *argv[]) {
	bool newline, interpret;
	int i;

	newline = true;
	interpret = false;
	optind = 0; /* extern */

	if (setenv("POSIXLY_CORRECT", "", 0))
		fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));

	while ((i = getopt(argc, argv, "neE")) != -1) {
		switch (i) {
			case 'n': newline = false; break;
			case 'e': interpret = true; break;
			case 'E': interpret = false; break;
			default: return 1;
		}
	}

	if ((i = optind) && strcmp(argv[i - 1], "--") == 0)
		--i;

	for (; i < argc; ++i) {
		bool escape, is_hex;
		size_t length;
		int count, num;

		length = strlen(argv[i]);
		escape = false;
		count = 0;
		num = 0;

		for (size_t j = 0; j < length; ++j) {
			char c = argv[i][j];
			if (!escape) {
				if (interpret && c == '\\') {
					escape = true;
					continue;
				}
			} else {
				if (count) {
					int tmp;
					if ((tmp = get_num(c, is_hex)) != -1) {
						num *= is_hex ? 16 : 8;
						num += tmp;
						--count;
					} else {
						count = 0;
					}

					if (!count) {
						putc(num, stdout);
						num = 0;
						escape = false;
						if (tmp == -1)
							putc(c, stdout);
					}
					continue;
				}

				switch (c) {
					case '\\': break;
					case 'a': c = '\a'; break;
					case 'b': c = '\b'; break;
					case 'c': return 0;
					case 'e': c = '\x1b'; break;
					case 'f': c = '\f'; break;
					case 'n': c = '\n'; break;
					case 'r': c = '\r'; break;
					case 't': c = '\t'; break;
					case 'v': c = '\v'; break;
					case '0':
						is_hex = false;
						count = 3;
						continue;
					case 'x':
						is_hex = true;
						count = 2;
						continue;
					default: printf("\\%c", c); continue;
				}
				escape = false;
			}
			putc(c, stdout);
		}
		if (escape)
			putc('\\', stdout);

		if (i < argc - 1)
			putc(' ', stdout);
	}

	if (newline)
		putc('\n', stdout);

	return 0;
}

