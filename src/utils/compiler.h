#pragma once

#if defined(__GNUC__)
# define ATTR_UNUSED __attribute__((__unused__))
#else
# warning unsupported compiler: results may vary
# define ATTR_UNUSED
#endif
