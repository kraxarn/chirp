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

if (TARGET SDL3-shared)
	add_custom_command(
		TARGET ${EXEC_NAME} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:${EXEC_NAME}>
		VERBATIM
	)
endif ()
