# all options can be found on:
# https://github.com/raysan5/raylib/wiki/CMake-Build-Options

# build options
set(BUILD_SHARED_LIBS OFF)
set(CUSTOMIZE_BUILD ON)

# modules
set(SUPPORT_MODULE_RSHAPES ON)
set(SUPPORT_MODULE_RTEXTURES ON)
set(SUPPORT_MODULE_RTEXT ON)
set(SUPPORT_MODULE_RMODELS OFF)
set(SUPPORT_MODULE_RAUDIO ON)

# core
set(SUPPORT_CAMERA_SYSTEM ON)
set(SUPPORT_GESTURES_SYSTEM OFF)
set(SUPPORT_RPRAND_GENERATOR ON)
set(SUPPORT_MOUSE_GESTURES OFF)
set(SUPPORT_SSH_KEYBOARD_RPI OFF)
set(SUPPORT_DEFAULT_FONT ON)
set(SUPPORT_SCREEN_CAPTURE ON)
set(SUPPORT_GIF_RECORDING OFF)
set(SUPPORT_BUSY_WAIT_LOOP OFF)
set(SUPPORT_EVENTS_WAITING OFF)
set(SUPPORT_WINMM_HIGHRES_TIMER ON)
set(SUPPORT_COMPRESSION_API OFF)
set(SUPPORT_EVENTS_AUTOMATION OFF)
set(SUPPORT_CUSTOM_FRAME_CONTROL OFF)

# shapes
set(SUPPORT_QUADS_DRAW_MODE ON)

# textures
set(SUPPORT_IMAGE_EXPORT ON)
set(SUPPORT_IMAGE_GENERATION ON)
set(SUPPORT_IMAGE_MANIPULATION ON)
set(SUPPORT_FILEFORMAT_PNG ON)
set(SUPPORT_FILEFORMAT_DDS OFF)
set(SUPPORT_FILEFORMAT_HDR OFF)
set(SUPPORT_FILEFORMAT_PIC OFF)
set(SUPPORT_FILEFORMAT_PNM OFF)
set(SUPPORT_FILEFORMAT_KTX OFF)
set(SUPPORT_FILEFORMAT_ASTC OFF)
set(SUPPORT_FILEFORMAT_BMP OFF)
set(SUPPORT_FILEFORMAT_TGA OFF)
set(SUPPORT_FILEFORMAT_JPG OFF)
set(SUPPORT_FILEFORMAT_GIF OFF)
set(SUPPORT_FILEFORMAT_QOI OFF)
set(SUPPORT_FILEFORMAT_PSD OFF)
set(SUPPORT_FILEFORMAT_PKM OFF)
set(SUPPORT_FILEFORMAT_PVR OFF)
set(SUPPORT_FILEFORMAT_SVG OFF)

# text
set(SUPPORT_FILEFORMAT_FNT OFF)
set(SUPPORT_FILEFORMAT_TTF ON)
set(SUPPORT_TEXT_MANIPULATION ON)
set(SUPPORT_FONT_ATLAS_WHITE_REC ON)

# models
set(SUPPORT_MESH_GENERATION OFF)
set(SUPPORT_FILEFORMAT_OBJ OFF)
set(SUPPORT_FILEFORMAT_MTL OFF)
set(SUPPORT_FILEFORMAT_IQM OFF)
set(SUPPORT_FILEFORMAT_GLTF OFF)
set(SUPPORT_FILEFORMAT_VOX OFF)
set(SUPPORT_FILEFORMAT_M3D OFF)

# audio
set(SUPPORT_FILEFORMAT_WAV ON)
set(SUPPORT_FILEFORMAT_OGG OFF)
set(SUPPORT_FILEFORMAT_XM ON)
set(SUPPORT_FILEFORMAT_MOD OFF)
set(SUPPORT_FILEFORMAT_MP3 OFF)
set(SUPPORT_FILEFORMAT_QOA OFF)
set(SUPPORT_FILEFORMAT_FLAC OFF)

# utils
set(SUPPORT_STANDARD_FILEIO ON)
set(SUPPORT_TRACELOG ON)

include(FetchContent)

FetchContent_Declare(raylib
	GIT_REPOSITORY https://github.com/raysan5/raylib.git
	GIT_TAG 5.0)

message(STATUS "Downloading: raylib")
FetchContent_MakeAvailable(raylib)