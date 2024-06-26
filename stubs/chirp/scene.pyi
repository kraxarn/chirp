import abc
import typing

import chirp


class Scene(abc.ABC):
	@abc.abstractmethod
	def load(self) -> None:
		...

	@abc.abstractmethod
	def update(self, delta: float) -> None:
		...

	@abc.abstractmethod
	def draw(self) -> None:
		...

	@property
	def scenes(self) -> chirp.SceneManager:
		...

	@property
	def window(self) -> chirp.Window:
		...

	@property
	def assets(self) -> typing.Any:
		...
