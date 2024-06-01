#include "chirp/os.hpp"

auto chirp::os::is_debug() -> bool
{
#ifdef NDEBUG
	return false;
#else
	return true;
#endif
}
