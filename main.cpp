#include <iostream>
#include "include/argparse.hpp"
#include "src/compiler.hpp"

const std::string banner = " ____  __    __    __  ____ "
                           "(  __)(  )  (  )  (  )/ ___)"
                           " ) _) / (_/\\/ (_/\\ )( \\___ \\"
                           "(____)\\____/\\____/(__)(____/";

int handle_source_files(const std::vector<std::string>& files, const bool verbose) {
    auto c = Compiler(verbose);
    return c.compile(files);
}



int run_interpreter() {
    std::string expr;
    std::cout << banner << "\n";
    std::cout << "version 0.0.1\n";
    std::cout << "ellis>";
    return 0;
}

int main(int argc, char* argv[]) {
    argparse::ArgumentParser program("ellisc");

    program.add_argument("source_files")
            .remaining()
            .help("The list of input source files to be compiled");

    program.add_argument("--verbose")
            .default_value(false)
            .implicit_value(true);

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
    if (program["verbose"] == true)
        std::cout << files.size() << " source files provided" << std::endl;

    if (program["interpreter"] == true) {
        return run_interpreter();
    }

    if (files.empty()) {
        std::cerr << "No input files provided.";
        return 1;
    }

    handle_source_files(files, program["verbose"] == true);
    return 0;
}
