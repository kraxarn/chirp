#pragma once

#include <chirp/assets.hpp>
#include <chirp/window.hpp>

namespace chirp
{
	class scene_manager;
	class entity_container;

	class scene
	{
	public:
		explicit scene(const assets &assets);

		virtual ~scene();

		void init(chirp::scene_manager &scene);

		virtual void load();

		virtual void update(float delta);

		virtual void draw();

		[[nodiscard]]
		auto entities() const -> chirp::entity_container &;

	protected:
		[[nodiscard]]
		auto scenes() const -> chirp::scene_manager &;

		[[nodiscard]]
		auto window() const -> const window &;

	private:
		chirp::scene_manager *scene_manager;
		chirp::entity_container *entity_container;
		chirp::window window_manager;
	};
}
