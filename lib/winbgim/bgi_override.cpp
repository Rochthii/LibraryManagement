// provide our own definition of bgiout so the executable controls its construction
// This prevents pulling in the archive member that defines bgiout (and its static ctor)
// from libbgi.a which can cause static-initialization order issues.
#include <sstream>

// define the global exactly as declared in the winbgim headers
std::ostringstream bgiout;
