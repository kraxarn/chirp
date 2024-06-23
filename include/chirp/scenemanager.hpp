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
		void push(T *scene)
		{
			auto handle = std::shared_ptr<T>(scene);
			handle->init(*this);
			handle->load();
			scenes.push(handle);
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

		auto get_assets() const -> const chirp::assets &;

	private:
		std::stack<std::shared_ptr<chirp::scene>> scenes;

		chirp::assets assets;
	};
}
