# name - a single name used by the test, executable, and source
macro(AddTest name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PoGoCmp)
    add_test(${name} ${name})
    set_target_properties(${name} PROPERTIES FOLDER Tests)
endmacro()

AddTest(Test1)
AddTest(NameTest)
