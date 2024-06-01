#pragma once

#include <chirp/scene.hpp>

#include <stack>

namespace chirp
{
	class scene_manager
	{
	public:
		/**
		 * Push a new scene, making it the current active one
		 */
		template<typename T>
		void push()
		{
			scenes.push(std::make_shared<T>(assets));
		}

		/**
		 * Pop current scene, going back to the previous one,
		 * or closing the window if no previous scene
		 */
		void pop();

		/**
		 * Get current scene, or nullptr if no scene
		 */
		auto peek() -> std::shared_ptr<scene>;

	private:
		std::stack<std::shared_ptr<scene>> scenes;

		assets assets;
	};
}
