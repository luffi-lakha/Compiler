#include <fstream>
#include <iostream>
#include <sstream>
#include "parser.hpp"
#include "semantic.h"
#include "ir_generator.h"

using namespace cmini;

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: my_compiler <file> [ -o out.ll ]\n";
        return 1;
    }
    std::string inPath = argv[1];
    std::string outPath = "outputs/output.ll";
    for (int i=2;i<argc;i++) {
        std::string a = argv[i];
        if (a=="-o" && i+1<argc) { outPath = argv[++i]; }
    }

    std::ifstream in(inPath);
    if (!in) { std::cerr << "cannot open: " << inPath << "\n"; return 1; }
    std::unique_ptr<Program> prog;
    try {
        prog = parseFile(inPath);
    } catch (const std::exception& ex) {
        std::cerr << inPath << ": syntax error: " << ex.what() << "\n";
        return 1;
    }

    Semantic sem; sem.analyze(*prog);
    if (!sem.diags.ok()) {
        for (auto& m : sem.diags.messages) std::cerr << "error: " << m << "\n";
        return 1;
    }

    IRGen ir; std::string text = ir.gen(*prog);
    std::ofstream out(outPath);
    if (!out) { std::cerr << "cannot write: " << outPath << "\n"; return 1; }
    out << text;
    std::cout << "wrote " << outPath << "\n";
    return 0;
}
