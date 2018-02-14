add_executable(PoGoCmpCli main.cpp)
target_link_libraries(PoGoCmpCli PoGoCmp)
install(TARGETS PoGoCmpCli DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
