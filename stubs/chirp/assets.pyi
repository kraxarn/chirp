import typing
import chirp


class Assets:
	def music(self, path: str) -> chirp.Music:
		...

	def sound(self, path: str) -> chirp.Sound:
		...

	def font(self, path: str, font_size: int) -> chirp.Font:
		...

	def image(self, path: str) -> chirp.Image:
		...

	def tileset(self, path: str) -> typing.Any:
		...

	def level(self, path: str) -> chirp.Level:
		...
