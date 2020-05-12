#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command.h"

static char optstring[] = "diw:";

#ifdef _GNU_SOURCE
	#include <getopt.h>
static struct option longopts[] = {
    {"decode", no_argument, NULL, 'd'},
    {"ignore-garbage", no_argument, NULL, 'i'},
    {"wrap", required_argument, NULL, 'w'},
};
	#define GET_OPT(argc, argv, optstring) \
		getopt_long(argc, argv, optstring, longopts, NULL)
#else
	#define GET_OPT(argc, argv, optstring) getopt(argc, argv, optstring)
#endif

static int encode(int num) {
	if (num >= 0 && num <= 25)
		return 'A' + num;
	else if (num >= 26 && num <= 51)
		return 'a' + num - 26;
	else if (num >= 52 && num <= 61)
		return '0' + num - 52;
	else if (num == 62)
		return '+';
	else if (num == 63)
		return '/';
	return -1;
}

static int decode(int num) {
	if (num >= 'A' && num <= 'Z')
		return num - 'A';
	else if (num >= 'a' && num <= 'z')
		return num - 'a' + 26;
	else if (num >= '0' && num <= '9')
		return num - '0' + 52;
	else if (num == '+')
		return 62;
	else if (num == '/')
		return 63;
	return -1;
}

COMMAND(base64, int argc, char *argv[]) {
	FILE *file;
	char *ptr;
	bool use_decode, ignore_garb;
	long long wrap;
	int i, c, hold, mask, size;

	ptr = NULL;
	use_decode = false;
	ignore_garb = false;
	wrap = 76; /* default value (must not be 0) */
	i = 2;
	hold = 0;
	mask = 0;
	optind = 0; /* extern */

	while ((c = GET_OPT(argc, argv, optstring)) != -1) {
		switch (c) {
			case 'd': use_decode = true; break;
			case 'i': ignore_garb = true; break;
			case 'w': wrap = strtoll(optarg, &ptr, 10); break;
			default: return 1;
		}
		if (wrap < 0 || (!wrap && !ptr)) {
			fprintf(
			    stderr, "%s: invalid wrap size '%s'\n", argv[0],
			    optarg);
			return 1;
		}
	}

	size = argc - optind;

	if (size > 1) {
		fprintf(
		    stderr, "%s: extra operand '%s'\n", argv[0], argv[optind]);
		return 1;
	} else if (size == 0) {
		file = stdin;
	} else {
		file = fopen(argv[optind], "r");
	}

	if (!file) {
		fprintf(
		    stderr, "%s: %s: %s\n", argv[0], argv[optind],
		    strerror(errno));
		return 1;
	}

	if (!use_decode) {
		/* encoding */
		size_t index;
		for (index = 0; (c = fgetc(file)) != EOF;) {
			putc(encode(hold | (c >> i)), stdout);
			if (wrap && !(index = (index + 1) % wrap))
				putc('\n', stdout);
			mask = (1 << i) - 1;
			hold = (c & mask) << (6 - i);
			if ((i += 2) == 8) {
				putc(encode(hold), stdout);
				if (wrap && !(index = (index + 1) % wrap))
					putc('\n', stdout);
				hold = 0;
				i = 2;
			}
		}

		if (hold) {
			putc(encode(hold), stdout);
			for (c = 6; c > i / 2 + 1; c -= 2) {
				if (wrap && !(index = (index + 1) % wrap))
					putc('\n', stdout);
				putc('=', stdout);
			}
		}
		if (mask)
			putc('\n', stdout);
	} else {
		/* decoding */
		for (; (c = fgetc(file)) != EOF;) {
			if (c == '\n')
				continue;

			if (c == '=') {
				if (hold) {
					putc(hold, stdout);
					hold = 0;
				}
				if (i != 2) {
					if ((i += 2) == 10)
						i = 2;
					continue;
				}
				i = 0;
				break;
			}

			if ((c = decode(c)) == -1) {
				if (c == -1 && ignore_garb)
					continue;
				i = 0;
				break;
			}

			if (i != 2) {
				hold |= c >> (8 - i);
				putc(hold, stdout);
			}
			mask = (i << (8 - i)) - 1;
			hold = (c & mask) << i;

			if ((i += 2) == 10)
				i = 2;
		}

		if (i != 2) {
			fprintf(stderr, "%s: Invalid input\n", argv[0]);
			return 1;
		}
	}

	if (file != stdin)
		fclose(file);
	return 0;
}
