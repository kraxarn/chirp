import typing

import chirp


class Game:
	def __init__(self):
		...

	def run(self) -> None:
		...

	def exit(self) -> None:
		...

	def get_target_fps(self) -> int:
		...

	def set_target_fps(self, fps: int) -> None:
		...

	def get_clear_color(self) -> typing.Any:
		...

	def set_clear_color(self, color: typing.Any) -> None:
		...

	def window(self) -> chirp.Window:
		...

	def scenes(self) -> chirp.SceneManager:
		...
