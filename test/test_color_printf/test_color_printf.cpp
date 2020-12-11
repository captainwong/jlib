#include "../../jlib/ansi_color_codes.h"
#include <stdio.h>

int main()
{
	printf(RED_BOLD("RED_BOLD for critical messages!") "\n");
	printf(RED("RED for error messages!") "\n");
	printf(YELLOW("YELLOW for warning messages!") "\n");
	printf("This is normal color for infomation messages.\n");
	printf(BLACK_BOLD("BLACK_BOLD for debug messages.") "\n");
	printf(GREEN("GREEN for success messages.") "\n");

	return 0;
}
