#pragma once

namespace chirp
{
	enum class gamepad_button: unsigned char
	{
		unknown          = 0,
		left_face_up     = 1,
		left_face_right  = 2,
		left_face_down   = 3,
		left_face_left   = 4,
		right_face_up    = 5,
		right_face_right = 6,
		right_face_down  = 7,
		right_face_left  = 8,
		left_trigger_1   = 9,
		left_trigger_2   = 10,
		right_trigger_1  = 11,
		right_trigger_2  = 12,
		middle_left      = 13,
		middle           = 14,
		middle_right     = 15,
		left_thumb       = 16,
		right_thumb      = 17,
	};
}
