#pragma once

#include <chirp/assets.hpp>

namespace chirp
{
	class scene_manager;

	class scene
	{
	public:
		explicit scene(const assets &assets);

		void init(scene_manager &scene);

		virtual void load();

		virtual ~scene() = default;

		virtual void update(float delta) = 0;

		virtual void draw() = 0;

	protected:
		[[nodiscard]]
		auto scenes() const -> scene_manager &;

	private:
		scene_manager *scene_manager;
	};
}
