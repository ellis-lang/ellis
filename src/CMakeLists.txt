project(EllisLib)

find_package(LLVM REQUIRED CONFIG)


message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})


add_library(ellis STATIC compiler.hpp compiler.cpp
        lex.cpp
        lex.hpp
        source_handler.hpp
        ast.cpp
        ast.hpp
        parser.cpp
        parser.hpp
        codegen.cpp
        codegen.hpp)

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(ellis ${llvm_libs})