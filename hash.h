#pragma once

static unsigned long hash(const char *str) {
	unsigned long hash = 5381;
	unsigned char c;

	while ((c = *str++))
		hash = hash * 33 + c;

	return hash;
}
