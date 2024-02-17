#include <iostream>
#include "include/argparse.hpp"
#include "src/compiler.hpp"
#include "src/repl.hpp"

int handle_source_files(const std::vector<std::string>& files, const bool verbose) {
    auto c = Compiler(verbose);
    return c.compile(files);
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("ellisc");

    program.add_argument("source_files")
            .remaining()
            .default_value(std::vector<std::string>())
            .help("The list of input source files to be compiled");

    program.add_argument("--verbose")
            .help("Increase output verbosity.")
            .flag();

    program.add_argument("--interpreter")
        .default_value(false)
        .implicit_value(true);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    const auto files = program.get<std::vector<std::string>>("source_files");

    if (files.empty())
        return repl();

    return handle_source_files(files, program["verbose"] == true);
}
