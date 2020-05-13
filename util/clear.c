#include <stdio.h>

#include "command.h"

COMMAND(clear, void) {
	/* \033[H  - Move cursor to top left corner
	 * \033[2J - Clear entire screen
	 * \033[3J - Clear entire screen and scrollback buffer
	 */
	fputs("\033[H\033[2J\033[3J", stdout);
	return 0;
}
