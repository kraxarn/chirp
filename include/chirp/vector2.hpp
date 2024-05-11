#pragma once

namespace chirp
{
	/**
	 * 2-dimensional vector
	 * @tparam T Contained type
	 */
	template<typename T>
	class vector2
	{
	public:
		/**
		 * Construct a vector2 at 0,0
		 */
		vector2() : vector2(static_cast<T>(0), static_cast<T>(0))
		{
		}

		/**
		 * Construct a vector2 with specified values
		 * @param x X
		 * @param y Y
		 */
		vector2(const T x, const T y)
			: x_val(x), y_val(y)
		{
		}

		/**
		 * Convert a vector2 to another type
		 * @tparam Target Target type
		 * @return Vector2 of target type
		 */
		template<typename Target>
		auto to() const -> vector2<Target>
		{
			return {
				static_cast<Target>(x()),
				static_cast<Target>(y()),
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

		/**
		 * Multiply x and y with specified value
		 * @param value Value to multiply with
		 * @return New vector with multiplied values
		 */
		auto operator*(T value) const -> vector2<T>
		{
			return {x() * value, y() * value};
		}

		/**
		 * Divide x and y with specified value
		 * @param value Value to divide by
		 * @return New vector with divided values
		 */
		auto operator/(T value) const -> vector2<T>
		{
			return {x() / value, y() / value};
		}

		/**
		 * Add value to vector
		 * @param value Value to add
		 * @return New vector with added values
		 */
		auto operator+(T value) const -> vector2<T>
		{
			return vector2<T>(x() + value, y() + value);
		}

		/**
		 * Subtract value from vector
		 * @param value Value to subtract
		 * @return New vector with subtracted values
		 */
		auto operator-(T value) const -> vector2<T>
		{
			return vector2<T>(x() - value, y() - value);
		}

		/**
		 * Multiply x and y with x and y of specified value
		 * @param value Vector to multiply with
		 * @return New vector with multiplied values
		 */
		auto operator+(const vector2<T> &value) const -> vector2<T>
		{
			return {x() + value.x(), y() + value.y()};
		}

		auto operator=(const vector2 &value) -> vector2 &
		{
			if (this != &value)
			{
				this->x_val = value.x();
				this->y_val = value.y();
			}

			return *this;
		}

		auto operator+=(const vector2 &value) -> vector2 &
		{
			this->x_val += value.x();
			this->y_val += value.y();
			return *this;
		}

		auto operator-=(const vector2 &value) -> vector2 &
		{
			this->x_val -= value.x();
			this->y_val -= value.y();
			return *this;
		}

	private:
		T x_val;
		T y_val;
	};

	/**
	 * A vector2 of float values
	 */
	using vector2f = vector2<float>;

	/**
	 * A vector2 of int values
	 */
	using vector2i = vector2<int>;
}
