include(CTest)
enable_testing()

set(TESTS_TARGET "${PROJECT_NAME}_tests")

add_executable(${TESTS_TARGET}
	main.c
	testarray.c
)

# add_subdirectory(../engine chirp) # TODO: Main CMake file already includes chirp
target_link_libraries(${TESTS_TARGET} PRIVATE chirp)

add_test(NAME test_array COMMAND ${TESTS_TARGET} 1)

include(../deps/sdl3.cmake)
target_link_libraries(${TESTS_TARGET} PRIVATE SDL3::SDL3)

if (TARGET SDL3-shared)
	add_custom_command(
		TARGET ${TESTS_TARGET} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:${TESTS_TARGET}>
		VERBATIM
	)
endif ()
