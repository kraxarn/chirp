#pragma once

#include <memory>

namespace chirp
{
	/**
	 * Entity handle
	 */
	template<typename T>
	using entity = std::unique_ptr<T>;
}
