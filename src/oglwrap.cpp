/** @file oglwrap.cpp
    @brief The file to link if OGLWRAP_HEADER_ONLY is 0.
    It might grant a little speed boost.
*/

#include "oglwrap_config.hpp"

// The magic is in these line:
#undef OGLWRAP_HEADER_ONLY
#define OGLWRAP_HEADER_ONLY 1
#undef OGLWRAP_INSTATIATE_TEMPLATES
#define OGLWRAP_INSTATIATE_TEMPLATES 1
// This makes the headers instantiate the global variables, and the templates.
// But the other .cpp files will include these headers with OGLWRAP_HEADER_ONLY
// defined to be 0, and they will reference the objects defined here.

#include "oglwrap/glew.hpp"
#include "oglwrap/oglwrap.hpp"
