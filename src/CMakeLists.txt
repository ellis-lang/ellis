project(EllisLib)

find_package(LLVM REQUIRED CONFIG)


message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})


include_directories(/usr/include/readline)

add_library(ellis STATIC compiler.hpp compiler.cpp
        lex.cpp
        lex.hpp
        source_handler.hpp
        ast.cpp
        ast.hpp
        parser.cpp
        parser.hpp
        codegen.cpp
        codegen.hpp
        repl.hpp
        ellis_jit.hpp
        #ast_visitor.h
        #codegen_visitor.cpp
        #codegen_visitor.h
        #abstract_syntax_tree.cpp
        #abstract_syntax_tree.h
)

llvm_map_components_to_libnames(llvm_libs support core irreader orcjit target support passes native)

target_link_libraries(ellis ${llvm_libs} readline)