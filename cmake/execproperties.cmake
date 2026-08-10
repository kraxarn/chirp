function (set_target_exec_properties target)
	cmake_parse_arguments(arg "" "IDENTIFIER;VERSION_LONG;VERSION_SHORT" "" ${ARGN})

	set_target_properties(${target} PROPERTIES
		# macOS / iOS
		MACOSX_BUNDLE TRUE
		MACOSX_BUNDLE_GUI_IDENTIFIER "${arg_IDENTIFIER}"
		MACOSX_BUNDLE_BUNDLE_VERSION "${arg_VERSION_LONG}"
		MACOSX_BUNDLE_SHORT_VERSION_STRING "${arg_VERSION_SHORT}"
		# Windows
		WIN32_EXECUTABLE TRUE
	)
endfunction ()
