class Vector2[T]:
	def __init__(self, x: T = 0, y: T = 0):
		...

	@property
	def x(self) -> T:
		...

	@property
	def y(self) -> T:
		...

	def __add__(self, other: Vector2[T] | T) -> Vector2[T]:
		...


class Vector2i(Vector2[int]):
	...


class Vector2f(Vector2[float]):
	...
