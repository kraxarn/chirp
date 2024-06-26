import typing


class Window:
	@property
	def title(self) -> str:
		...

	@title.setter
	def title(self, title: str) -> None:
		...

	@property
	def size(self) -> typing.Any:
		...

	@size.setter
	def size(self, size: typing.Any) -> None:
		...
