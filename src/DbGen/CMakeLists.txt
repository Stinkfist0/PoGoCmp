add_executable(PoGoDbGen PoGoDbGen.cpp)
target_link_libraries(PoGoDbGen PoGoCmp --coverage))
install(TARGETS PoGoDbGen DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
set_target_properties(PoGoDbGen PROPERTIES FOLDER PoGoCmp)
