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
			auto scene = std::make_shared<T>(assets);
			scene->init(*this);
			scenes.push(scene);
		}

		/**
		 * Pop current scene, going back to the previous one,
		 * or closing the window if no previous scene
		 */
		void pop();

		/**
		 * Get current scene, or nullptr if no scene
		 */
		auto peek() const -> std::shared_ptr<chirp::scene>;

	private:
		std::stack<std::shared_ptr<chirp::scene>> scenes;

		chirp::assets assets;
	};
}
