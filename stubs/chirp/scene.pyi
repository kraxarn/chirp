import abc

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

	def scenes(self) -> chirp.SceneManager:
		...

	def window(self) -> chirp.Window:
		...
