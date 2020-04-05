#include <stdlib.h>
#include <stdio.h>

#include "api.hpp"
#include "map.hpp"

int main (int argc, const char** argv)
{
	auto&& s7 = S7::get();
	s7.def_var ("API_APPLE", static_cast<int> (What::APPLE));
	if (argc < 2) {
		fputs ("usage: mazeai scm_file", stderr);
		return -1;
	}
	const char* scm_path = argv [1];
	S7::get().load (scm_path);
}

