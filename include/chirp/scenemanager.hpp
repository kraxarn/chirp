#pragma once

#include <chirp/scene.hpp>
#include <chirp/internal/sceneaction.hpp>

#include <queue>
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
		 * Push a new scene at the end of the frame, making it the current active one
		 */
		void queue_push(chirp::scene *scene);

		/**
		 * Pop current scene at the end of the frame, going back to the previous one,
		 * or closing the window if no previous scene
		 */
		void queue_pop();

		/**
		 * Check if there are no scenes
		 */
		[[nodiscard]]
		auto empty() const -> bool;

		/**
		 * Get current scene
		 */
		[[nodiscard]]
		auto peek() const -> chirp::scene &;

		auto get_assets() const -> const chirp::assets &;

		void clear_queue();

	private:
		std::stack<std::shared_ptr<chirp::scene>> scenes;
		std::queue<internal::scene_action> queue;
		chirp::assets assets;
	};
}
