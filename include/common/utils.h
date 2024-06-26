#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <stdlib.h>

#define FAIL(msg) do{std::cerr << msg << std::endl; return EXIT_FAILURE;}while(0)

#ifdef DEBUG
#define DBOUT(x) std::cout << x << '\n'
#else
#define DBOUT(x)
#endif

#endif