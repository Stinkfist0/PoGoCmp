add_executable(PoGoDbGen PoGoDbGen.cpp)

target_link_libraries(PoGoDbGen PoGoCmp nlohmann_json)
# including nlohmann_json adds -std=gnu++11 which causes the compilation to fail
if (NOT MSVC)
    set_property(TARGET PoGoDbGen PROPERTY CXX_STANDARD 17)
endif()

if (ENABLE_COVERAGE)
    target_link_libraries(PoGoDbGen --coverage)
endif()

install(TARGETS PoGoDbGen DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

set_target_properties(PoGoDbGen PROPERTIES FOLDER PoGoCmp)
