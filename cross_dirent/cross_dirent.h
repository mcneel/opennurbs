#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

// if not windows use the posix dirent.h
#include <dirent.h>

#else

// use dirent windows
#include "./include/dirent.h"

#endif