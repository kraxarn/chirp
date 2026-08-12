set(EXEC_NAME "chirp3d")

if (ANDROID)
	add_library(${EXEC_NAME} SHARED)
else ()
	add_executable(${EXEC_NAME})
endif ()

add_subdirectory(src)

target_include_directories(${EXEC_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_compile_definitions(${EXEC_NAME} PRIVATE
	ENGINE_NAME="${EXEC_NAME}"
	ENGINE_VERSION="${PROJECT_VERSION}"
	ENGINE_IDENTIFIER="com.kraxarn.${EXEC_NAME}"
	ENGINE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
	ENGINE_COMPILER="${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}"
)

target_link_libraries(${EXEC_NAME} PRIVATE
	chirp
	pocketpy
)

include(../cmake/resources.cmake)
target_resources(${EXEC_NAME})

include(../cmake/execproperties.cmake)
set_target_exec_properties(${EXEC_NAME}
	IDENTIFIER    "com.kraxarn.${EXEC_NAME}"
	VERSION_LONG  "${PROJECT_VERSION}"
	VERSION_SHORT "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

include(../cmake/lto.cmake)
target_enable_lto(${EXEC_NAME})

include(../cmake/copysdl3.cmake)
target_copy_sdl3(${EXEC_NAME})
