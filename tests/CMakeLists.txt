# name - the name used as the names of the test, executable, and source file.
macro(AddTest name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PoGoCmp)
    if (ENABLE_COVERAGE)
        target_link_libraries(${name} --coverage)
    endif()
    add_test(${name} ${name})
    set_target_properties(${name} PROPERTIES FOLDER Tests)
endmacro()

AddTest(Test1)
AddTest(NameTest)
AddTest(StringUtilsTest)
