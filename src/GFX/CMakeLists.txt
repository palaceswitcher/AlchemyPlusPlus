add_library (GFX
	Animation.cpp
	Button.cpp
	Resource.cpp
	Text.cpp
	Rendering.cpp
	GraphicsContext.cpp)

target_link_libraries(GFX PRIVATE Game)
target_link_libraries(GFX PRIVATE SDL3::SDL3)
target_link_libraries(GFX PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

install (TARGETS GFX DESTINATION bin)
install (FILES Animation.hpp DESTINATION include)
install (FILES Button.hpp DESTINATION include)
install (FILES Resource.hpp DESTINATION include)
install (FILES Text.hpp DESTINATION include)
install (FILES Rendering.hpp DESTINATION include)
install (FILES GraphicsContext.hpp DESTINATION include)
