#pragma once

#include <memory>

namespace chirp
{
	/**
	 * Entity handle
	 */
	template<typename T>
	using handle = std::shared_ptr<T>;
}
