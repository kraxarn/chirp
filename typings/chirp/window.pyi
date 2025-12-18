import chirp

class Window:
	@property
	def title(self) -> str: ...

	@title.setter
	def title(self, value: str) -> None: ...

	@property
	def size(self) -> chirp.Vector2[int]: ...

	@size.setter
	def size(self, value: chirp.Vector2[int]) -> None: ...
