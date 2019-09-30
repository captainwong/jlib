#include "../../jlib/util/rand.h"
#include <stdio.h>

int main()
{
	printf("%s\n", jlib::nonce().data());
}
