import typing

import chirp


class Game:
	def __init__(self):
		...

	def run(self) -> None:
		...

	def exit(self) -> None:
		...

	@property
	def target_fps(self) -> int:
		...

	@target_fps.setter
	def target_fps(self, fps: int) -> None:
		...

	@property
	def clear_color(self) -> chirp.Color:
		...

	@clear_color.setter
	def clear_color(self, color: chirp.Color) -> None:
		...

	@property
	def window(self) -> chirp.Window:
		...

	@property
	def scenes(self) -> chirp.SceneManager:
		...
