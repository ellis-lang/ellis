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
    while ((buf = readline(">> ")) != nullptr) {
        if (strlen(buf) > 0) {
            add_history(buf);
        }

        printf("[%s]\n", buf);
        auto str = std::string(buf);
        c.jit(str);

        // readline malloc's a new buffer every time.
        free(buf);
    }
    return 0;
}

#endif //REPL_HPP
