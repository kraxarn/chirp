#pragma once

#include <memory>

namespace chirp
{
	/**
	 * Entity handle
	 */
	template<typename T>
	using handle = std::unique_ptr<T>;
}
