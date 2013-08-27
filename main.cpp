#include <iostream>
#include "embed.h"

int main(int, char**)
{
	embed_startup(__func__);
	zend_first_try {

	}
	zend_catch {
		std::cerr << "Bail out" << std::endl;
	}
	zend_end_try();
	embed_shutdown();

	return 0;
}
