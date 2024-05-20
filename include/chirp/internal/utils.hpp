#pragma once

#include "chirp/color.hpp"
#include "chirp/rectangle.hpp"
#include "chirp/vector2.hpp"

#include "raylib.h"

namespace chirp::internal
{
	class utils
	{
	public:
		static auto to_r_color(const color &color) -> Color;

		template<typename T>
		static auto to_r_vector2(const vector2<T> &vec) -> Vector2
		{
			return Vector2{
				.x = static_cast<float>(vec.x()),
				.y = static_cast<float>(vec.y()),
			};
		}

		template<typename T>
		static auto from_r_vector2(const Vector2 &vec) -> vector2<T>
		{
			return {
				static_cast<T>(vec.x),
				static_cast<T>(vec.y),
			};
		}

		template<typename T>
		static auto to_r_rectangle(const rectangle<T> &rec) -> Rectangle
		{
			return {
				.x = static_cast<float>(rec.x()),
				.y = static_cast<float>(rec.y()),
				.width = static_cast<float>(rec.width()),
				.height = static_cast<float>(rec.height()),
			};
		}

	private:
		utils() = default;
	};
}
