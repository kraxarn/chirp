function (target_copy_sdl3 target)
	if (TARGET SDL3-shared)
		add_custom_command(
			TARGET ${target} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:${target}>
			VERBATIM
		)
	endif ()
endfunction ()
