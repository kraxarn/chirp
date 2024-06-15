#pragma once

#include <chirp/entity.hpp>

#include <string>
#include <vector>

struct Music;

namespace chirp
{
	class music: public entity
	{
	public:
		explicit music(const std::vector<unsigned char> &data, std::string name);
		~music() override;

		void update(const chirp::scene &scene, float delta) override;
		void draw() const override;

		void play() const;

		[[nodiscard]]
		auto is_playing() const -> bool;

		void stop() const;

		void pause() const;

		void resume() const;

		[[nodiscard]]
		auto length() const -> float;

		[[nodiscard]]
		auto played() const -> float;

		[[nodiscard]]
		auto name() const -> const std::string &;

		[[nodiscard]]
		auto get_volume() const -> float;

		void set_volume(float value) const;

	private:
		Music *r_music;
		std::string file_name;
		float volume;

		static auto guess_file_type(const std::vector<unsigned char> &data) -> const char *;
	};
}
