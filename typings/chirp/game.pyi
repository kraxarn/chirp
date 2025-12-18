import chirp


class Game:
	def __init__(self, name: str, version: str, identifier: str, creator: str, type: str = "game"): ...

	@property
	def window(self) -> chirp.Window: ...
