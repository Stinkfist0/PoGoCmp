add_executable(PoGoCmpDbGen main.cpp)
target_link_libraries(PoGoCmpDbGen PoGoCmp)
install(TARGETS PoGoCmpDbGen DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
