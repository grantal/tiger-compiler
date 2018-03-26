#define CATCH_CONFIG_MAIN
#include <memory>
#include "catch.hpp"
#include "semantic_checks.hh"
#include "tiger.tab.h"
#include "buffman.hh"

extern FILE* yyin;

namespace tiger {

extern std::shared_ptr<ParentASTNode> programNode;

TEST_CASE("example semantic check","[semantics]") {
    auto b = buffman::Buffman("let type myInt = int var myVar : myInt := 1 in myVar end");
    REQUIRE(yyparse() == 0);
    REQUIRE(semantic_checks(programNode.get()) == 0);
}

TEST_CASE("test semantics for appel's testfiles","[semantics]") {

    // the keys are the numbers of the files that should error and the values are
    // the line numbers of those errors
    std::map<int, int> errFiles = {
        { 9, 3},
        {10, 2},
        {11, 2},
        {13, 2},
        {14, 2},
        {15, 2},
        {16, 2},
        {17, 2},
        {18, 2},
        {19, 2},
        {20, 2},
        {21, 2},
        {22, 2},
        {23, 2},
        {24, 2},
        {25, 2},
        {26, 2},
        {28, 2},
        {29, 2},
        {31, 2},
        {32, 2},
        {33, 2},
        {34, 2},
        {35, 2},
        {36, 2},
        {40, 2},
        {45, 2},
        {49, 2}
    }; 

    for (int i = 1; i <= 48; i++) {
        char filename[26];
        sprintf(filename, "tiger-programs/test%d.tig", i);
        yyin = fopen(filename, "r");
        auto b = buffman::Buffman(yyin);
        if (errFiles.find(i) == errFiles.end()){
            /*
            SECTION("semantics for test" + std::to_string(i) + ".tig") {
                REQUIRE(yyparse() == 0);
                REQUIRE(semantic_checks(programNode.get()) == 0);
            }
            */
        } else {
            SECTION("make sure test" + std::to_string(i) + ".tig errors") {
                std::stringstream buffer;
                std::streambuf * old = std::cerr.rdbuf(buffer.rdbuf());
                REQUIRE(yyparse() == 0);
                REQUIRE(semantic_checks(programNode.get()) != 0);
                // make sure line number of the error is in the message
                REQUIRE(buffer.str().find(std::to_string(errFiles.at(i))) != std::string::npos);
                std::cerr.rdbuf(old);
            }

        }
    }
}

} //namespace
