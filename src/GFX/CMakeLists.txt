add_library (GFX
	Animation.cpp
	Button.cpp
	Resource.cpp
	Text.cpp
	Rendering.cpp)

target_link_libraries(GFX PRIVATE Game)

install (TARGETS GFX DESTINATION bin)
install (FILES Animation.hpp DESTINATION include)
install (FILES Button.hpp DESTINATION include)
install (FILES Resource.hpp DESTINATION include)
install (FILES Text.hpp DESTINATION include)
install (FILES Rendering.hpp DESTINATION include)
