function(enable_warnings target)
	if (ENABLE_WARNINGS_AS_ERRORS)
		target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
		if (CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
			target_compile_options(${target} PRIVATE
				-Wno-gnu-pointer-arith				# void* arithmetics in arrays
				-Wno-cast-function-type-mismatch	# Some hacky casting in ECS threads
			)
		elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
			target_compile_options(${target} PRIVATE
				-Wno-pointer-arith		# void* arithmetics in arrays
				-Wno-cast-function-type	# Some hacky casting in ECS threads
				-Wno-pedantic			# Map cleanup casts pointers to/from functions
			)
		endif ()
	endif ()
endfunction ()
