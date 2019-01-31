add_executable(PoGoCmpCli main.cpp ProgramOptions.h)
target_link_libraries(PoGoCmpCli PoGoCmp --coverage)
install(TARGETS PoGoCmpCli DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
set_target_properties(PoGoCmpCli PROPERTIES FOLDER PoGoCmp)
