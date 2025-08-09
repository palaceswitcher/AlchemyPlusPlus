cmake_minimum_required(VERSION 3.28)
project(AlchemyPlusPlus)

# Build options
option(BUILD_STATICSTD "Statically compile standard library" OFF)
option(SDL_VENDORED "Use vendored SDL libraries" OFF) #Switch between a system sdl library and a vendored sdl library
option(BUILD_DEBUG "Create debug build" ON)
option(NATIVE_OPTIMIZE "Optimize for native CPU" OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build settings
if(BUILD_DEBUG)
	set(CMAKE_BUILD_TYPE Debug)
	set(CMAKE_DEBUG_POSTFIX d)
else()
	set(CMAKE_BUILD_TYPE Release)
	set(CMAKE_CXX_FLAGS_RELEASE "-O2")
endif()
if(NATIVE_OPTIMIZE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

if(SDL_VENDORED)
	add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
	# 1. Look for a SDL3 package, 2. look for the SDL3 component and 3. fail if none can be found
	find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

# Find dependencies
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_ttf REQUIRED)
include_directories(${SDL3_INCLUDE_DIRS} ${SDL3_IMAGE_INCLUDE_DIRS} ${SDL3_TTF_INCLUDE_DIRS})

add_library(ImGui
	include/imgui/imgui.cpp
	include/imgui/imgui_demo.cpp
	include/imgui/imgui_draw.cpp
	include/imgui/imgui_tables.cpp
	include/imgui/imgui_widgets.cpp
	include/imgui/misc/cpp/imgui_stdlib.cpp
	include/imgui/backends/imgui_impl_sdl3.cpp
	include/imgui/backends/imgui_impl_sdlrenderer3.cpp
	include/imgui/backends/imgui_impl_opengl2.cpp
)

target_link_libraries(ImGui SDL3::SDL3)

# Include source directories and add executable
include_directories(src/Core)
include_directories(src/GFX)
include_directories(src/Game)
include_directories(src/Misc)
include_directories(src/UI)
include_directories(include/imgui)
include_directories(include/imgui/misc/cpp)
include_directories(include/imgui/backends)
add_subdirectory(include/glaze)
add_subdirectory(src/Core)
add_subdirectory(src/GFX)
add_subdirectory(src/Game)
add_subdirectory(src/Misc)
add_subdirectory(src/UI)

add_executable(AlchemyPlusPlus src/Core/Main.cpp) # Main file

if (BUILD_STATICSTD)
	target_link_options(AlchemyPlusPlus PRIVATE -static-libgcc -static-libstdc++)
endif(BUILD_STATICSTD)

if(WIN32)
	target_link_options(AlchemyPlusPlus PRIVATE "-mwindows") # Disable console pop-up
	target_link_options(AlchemyPlusPlus PRIVATE -static) # Statically link non-SDL libraries
endif(WIN32)

target_link_libraries(AlchemyPlusPlus SDL3::SDL3)
target_link_libraries(AlchemyPlusPlus SDL3::SDL3 SDL3_image::SDL3_image)
target_link_libraries(AlchemyPlusPlus SDL3::SDL3 SDL3_ttf::SDL3_ttf)
target_link_libraries(AlchemyPlusPlus ImGui)
target_link_libraries(AlchemyPlusPlus glaze::glaze)
target_link_libraries(AlchemyPlusPlus Core)
target_link_libraries(AlchemyPlusPlus GFX)
target_link_libraries(AlchemyPlusPlus Game)
target_link_libraries(AlchemyPlusPlus Misc)
target_link_libraries(AlchemyPlusPlus UI)
