import typing


class Assets:
	def music(self, path: str) -> typing.Any:
		...

	def sound(self, path: str) -> typing.Any:
		...

	def font(self, path: str, font_size: int) -> typing.Any:
		...

	def image(self, path: str) -> typing.Any:
		...

	def tileset(self, path: str) -> typing.Any:
		...

	def level(self, path: str) -> typing.Any:
		...
