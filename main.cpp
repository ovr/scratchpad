#include <istream>
#include <memory>
#include <cstdlib>
#include <cpptest.h>
#include "tests/Test_PZVal.h"

int main(int, char**)
{
	std::ios_base::sync_with_stdio(true);
	Test::Suite ts;
	ts.add(std::auto_ptr<Test::Suite>(new Test_PZVal()));
	Test::TextOutput output(Test::TextOutput::Verbose);

	return ts.run(output) ? EXIT_SUCCESS : EXIT_FAILURE;;
}
