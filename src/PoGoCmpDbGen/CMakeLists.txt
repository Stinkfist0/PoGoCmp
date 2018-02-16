add_executable(PoGoCmpDbGen main.cpp)
target_link_libraries(PoGoCmpDbGen PoGoCmp)
install(TARGETS PoGoCmpDbGen DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
set_target_properties(PoGoCmpDbGen PROPERTIES FOLDER PoGoCmp)
