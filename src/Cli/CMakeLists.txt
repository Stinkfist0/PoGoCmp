if (MSVC)
    # ranges-v3 is not W4-clean yet, supress the biggest spammers
    add_compile_options(/wd4459)
endif()

add_executable(PoGoCmpCli main.cpp ProgramOptions.h)

target_link_libraries(PoGoCmpCli PoGoCmp range-v3)
if (ENABLE_COVERAGE)
    target_link_libraries(PoGoCmpCli --coverage)
endif()


install(TARGETS PoGoCmpCli DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

set_target_properties(PoGoCmpCli PROPERTIES FOLDER PoGoCmp)
