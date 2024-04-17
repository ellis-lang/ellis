//
// Created by jonathan on 2/17/24.
//

#ifndef REPL_HPP
#define REPL_HPP

#include <readline/readline.h>
#include <readline/history.h>


const std::string banner = " ____  __    __    __  ____ \n"
                           "(  __)(  )  (  )  (  )/ ___)\n"
                           " ) _) / (_/\\/ (_/\\ )( \\___ \\\n"
                           "(____)\\____/\\____/(__)(____/\n";

int print_banner() {
    std::string expr;
    std::cout << banner;
    std::cout << "Ellis 0.0.1\n";
    return 0;
}

int repl() {
    print_banner();
    char* buf;
    auto c = Compiler(true);
    bool expr_complete = false;
    const auto prompt = ">> ";
    auto incomplete_prompt = "   ";
    while ((buf = readline(prompt)) != nullptr) {
        if (strlen(buf) > 0) {
            add_history(buf);
        }

        printf("[%s]\n", buf);
        try {
            auto source = std::string(buf);
            source.erase(source.find_last_not_of(" \n\r\t")+1);
            c.jit(source);

        } catch (const ParsingException& e) {
            std::cout << e.what() << std::endl;
        } catch (const CodeGenerationException& e) {
            std::cout << e.what() << std::endl;
        }

        // readline malloc's a new buffer every time.
        free(buf);
    }
    return 0;
}

#endif //REPL_HPP
