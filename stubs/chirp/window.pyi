import typing

import chirp


class Window:
	@property
	def title(self) -> str:
		...

	@title.setter
	def title(self, title: str) -> None:
		...

	@property
	def size(self) -> chirp.Vector2i:
		...

	@size.setter
	def size(self, size: chirp.Vector2i) -> None:
		...
