import typing

import chirp


class SceneManager:
	def push(self, scene: typing.Type[chirp.Scene]):
		...

	def pop(self) -> None:
		...

	def peek(self) -> typing.Any:
		...
