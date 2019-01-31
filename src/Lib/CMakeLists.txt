set(headers PoGoCmp.h PoGoDb.h StringUtils.h Utf8.h MinimalWin32.h MathUtils.h)
set(sources PoGoCmp.cpp Utf8.cpp)

if (BUILD_SHARED_LIBS)
    add_definitions(-DPOGOCMP_EXPORTS)
endif()

add_library(PoGoCmp ${headers} ${sources})
target_link_libraries(PoGoCmp --coverage)

install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS PoGoCmp
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

set_target_properties(PoGoCmp PROPERTIES FOLDER PoGoCmp)
