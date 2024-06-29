#pragma once

#include <memory>

namespace chirp
{
	/**
	 * Entity handle
	 */
#ifdef NO_MANAGE_MEMORY
	template<typename T>
	using handle = T *;
#else
	template<typename T>
	using handle = const std::shared_ptr<T>;
#endif
}
