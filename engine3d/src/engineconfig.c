#include "engineconfig.h"

engine_config_t engine_config_get()
{
	return (engine_config_t){
		.name = ENGINE_NAME,
		.version = ENGINE_VERSION,
		.identifier = ENGINE_IDENTIFIER,
		.build_type = ENGINE_BUILD_TYPE,
		.compiler = ENGINE_COMPILER,
#ifdef NDEBUG
		.debug = false,
#else
		.debug = true,
#endif
	};
}
