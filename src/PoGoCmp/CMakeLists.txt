set(headers PoGoCmp.h PoGoCmpDb.h)
set(sources PoGoCmp.cpp)

if (BUILD_SHARED_LIBS)
    add_definitions(-DPOGOCMP_EXPORTS)
endif()

add_library(PoGoCmp ${headers} ${sources})

install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(TARGETS PoGoCmp
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
)

set_target_properties(PoGoCmp PROPERTIES FOLDER PoGoCmp)
