class Level:
	def set_tile(self, x: int, y: int, value: int) -> None:
		...

	@property
	def name(self) -> str:
		...

	@property
	def tileset(self) -> str:
		...

	@property
	def music(self) -> str:
		...

	@property
	def map(self) -> list[list[int]]:
		...

	@property
	def type(self) -> str:
		...
