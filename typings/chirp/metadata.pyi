import enum


class Metadata:
	class Type(enum.Enum):
		GAME = "game"
		MEDIA_PLAYER = "mediaplayer"
		APPLICATION = "application"

	@property
	def name(self) -> str: ...

	@name.setter
	def name(self, value: str) -> None: ...

	@property
	def version(self) -> str: ...

	@version.setter
	def version(self, value: str) -> None: ...

	@property
	def identifier(self) -> str: ...

	@identifier.setter
	def identifier(self, value: str) -> None: ...

	@property
	def creator(self) -> str: ...

	@creator.setter
	def creator(self, value: str) -> None: ...

	@property
	def type(self) -> Type: ...

	@type.setter
	def type(self, value: Type) -> None: ...
