#pragma once

#include <chirp/assets.hpp>
#include <chirp/window.hpp>

namespace chirp
{
	class scene_manager;

	class scene
	{
	public:
		explicit scene(const assets &assets);

		void init(chirp::scene_manager &scene);

		virtual void load();

		virtual ~scene() = default;

		virtual void update(float delta) = 0;

		virtual void draw() = 0;

	protected:
		[[nodiscard]]
		auto scenes() const -> chirp::scene_manager &;

		[[nodiscard]]
		auto window() const -> const window &;

	private:
		chirp::scene_manager *scene_manager;
		chirp::window window_manager;
	};
}
