#pragma once

#include <memory>

namespace chirp
{
	/**
	 * Asset handle
	 */
	template<typename T>
	using asset = std::shared_ptr<T>;
}
