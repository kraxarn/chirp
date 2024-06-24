#pragma once

#include <chirp/assets.hpp>
#include <chirp/entitycontainer.hpp>
#include <chirp/window.hpp>

namespace chirp
{
	class scene_manager;

	class scene: public entity_container
	{
	public:
		explicit scene();

		virtual ~scene() = default;

		void init(chirp::scene_manager &scene);

		virtual void load() = 0;

		virtual void update(float delta);

		virtual void draw();

		[[nodiscard]]
		auto scenes() const -> chirp::scene_manager &;

		[[nodiscard]]
		auto window() const -> const window &;

		[[nodiscard]]
		auto assets() const -> const assets &;

	private:
		chirp::scene_manager *scene_manager;
		chirp::window window_manager;
	};
}
