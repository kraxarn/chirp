#pragma once

namespace chirp
{
	template<typename T>
	class rectangle
	{
	public:
		rectangle() : rectangle(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0))
		{
		}

		rectangle(const T x, const T y, const T width, const T height)
			: x_val(x),
			y_val(y),
			width_val(width),
			height_val(height)
		{
		}

		template<typename Target>
		auto to() const -> rectangle<Target>
		{
			return {
				static_cast<Target>(x()),
				static_cast<Target>(y()),
				static_cast<Target>(width()),
				static_cast<Target>(height()),
			};
		}

		[[nodiscard]]
		auto x() const -> T
		{
			return x_val;
		}

		[[nodiscard]]
		auto y() const -> T
		{
			return y_val;
		}

		[[nodiscard]]
		auto width() const -> T
		{
			return width_val;
		}

		[[nodiscard]]
		auto height() const -> T
		{
			return height_val;
		}

		auto operator+(const rectangle &value) const -> rectangle
		{
			return {
				x() + value.x(),
				y() + value.y(),
				width() + value.width(),
				height() + value.height(),
			};
		}

		auto operator+=(const rectangle &value) -> rectangle &
		{
			this->x_val += value.x();
			this->y_val += value.y();
			this->width_val += value.width();
			this->height_val += value.height();
			return *this;
		}

	private:
		T x_val;
		T y_val;
		T width_val;
		T height_val;
	};
}
