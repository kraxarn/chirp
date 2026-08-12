include(CTest)
enable_testing()

set(EXEC_NAME "chirp_tests")

add_executable(${EXEC_NAME}
	main.c
	testarray.c
)

add_test(NAME test_array COMMAND ${EXEC_NAME} 1)

target_link_libraries(${EXEC_NAME} PRIVATE
	SDL3::SDL3
	chirp
)

include(../cmake/copysdl3.cmake)
target_copy_sdl3(${EXEC_NAME})
