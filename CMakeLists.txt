cmake_minimum_required(VERSION 3.28)
project(AlchemyPlusPlus)

# Create an option to switch between a system sdl library and a vendored sdl library
option(MYGAME_VENDORED "Use vendored libraries" OFF)

# Build settings
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_DEBUG_POSTFIX d)
#set(CMAKE_CXX_FLAGS_RELEASE "-O2")

if(MYGAME_VENDORED)
    add_subdirectory(vendored/sdl EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

    # 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
    find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
endif()

# Find dependencies
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

# Include source directories and add executable
include_directories(src/JSON)
add_subdirectory(src/JSON)
include_directories(src/GFX)
add_subdirectory(src/GFX)
include_directories(src/Game)
add_subdirectory(src/Game)
include_directories(src/Misc)
add_subdirectory(src/Misc)
include_directories(src/Menu)
add_subdirectory(src/Menu)
# ImGui linking
include_directories(src/include/imgui)
include_directories(src/include/imgui/backends)
add_library(ImGui
    src/include/imgui/imgui.cpp
    src/include/imgui/imgui_demo.cpp
    src/include/imgui/imgui_draw.cpp
    src/include/imgui/imgui_tables.cpp
    src/include/imgui/imgui_widgets.cpp
    src/include/imgui/backends/imgui_impl_sdl2.cpp
    src/include/imgui/backends/imgui_impl_sdlrenderer2.cpp
    src/include/imgui/backends/imgui_impl_opengl2.cpp
)

add_executable(AlchemyPlusPlus src/main.cpp) #Main file

if(WIN32)
    target_link_options(AlchemyPlusPlus PRIVATE "-mwindows") #Disable console pop-up
    target_link_options(AlchemyPlusPlus PRIVATE -static) #Statically link non-SDL libraries
endif(WIN32)

# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(AlchemyPlusPlus SDL2::SDL2main)
endif()
target_link_libraries(AlchemyPlusPlus ImGui)
target_link_libraries(AlchemyPlusPlus JSON)
target_link_libraries(AlchemyPlusPlus GFX)
target_link_libraries(AlchemyPlusPlus Game)
target_link_libraries(AlchemyPlusPlus Misc)
target_link_libraries(AlchemyPlusPlus Menu)

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(AlchemyPlusPlus SDL2::SDL2 SDL2_image::SDL2_image)
target_link_libraries(AlchemyPlusPlus SDL2::SDL2 SDL2_ttf::SDL2_ttf)
