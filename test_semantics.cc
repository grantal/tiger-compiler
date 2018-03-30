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

TEST_CASE("Simple Record semantic check","[semantics]") {
    auto b = buffman::Buffman("let type myRec = {numTest:int} var myRecVar : myRec := myRec {numTest = 1} in myRecVar end");
    REQUIRE(yyparse() == 0);
    REQUIRE(semantic_checks(programNode.get()) == 0);
}

TEST_CASE("Record inside Record semantic check","[semantics]") {
    auto b = buffman::Buffman("let type myRec = {numTest:int} type myRec2 = {innerRec:myRec} var myRecVar : myRec := myRec {numTest = 5} var myRecVar2 : myRec2 := myRec2 {innerRec = myRecVar} in myRecVar.numTest end");
    REQUIRE(yyparse() == 0);
    REQUIRE(semantic_checks(programNode.get()) == 0);
}

TEST_CASE("Record declared inside Record semantic check","[semantics]") {
    auto b = buffman::Buffman("let type myRec = {numTest:int} type myRec2 = {innerRec:myRec} var myRecVar2 : myRec2 := myRec2 {innerRec = myRec{numTest = 5}} in myRecVar end");
    REQUIRE(yyparse() == 0);
    REQUIRE(semantic_checks(programNode.get()) == 0);
}

/*
TEST_CASE("Record declared inside Record declare inside Record semantic check","[semantics]") {
    auto b = buffman::Buffman("let type myRec = {numTest:int} type myRec2 = {innerRec:myRec} type myRec3 = {innerRec2:myRec2} var myRecVar3 : myRec3 := myRec3 {innerRec2 = myRec2 {innerRec = myRec {numTest = 5}}} in myRecVar3.innerRec2.innerRec.numTest end");
    REQUIRE(yyparse() == 0);
    std::cout << programNode->toStr() << std::endl;
    REQUIRE(semantic_checks(programNode.get()) == 0);
}
*/

TEST_CASE("test semantics for appel's testfiles","[semantics]") {

    // the keys are the numbers of the files that should error and the values are
    // the line numbers of those errors
    std::map<int, int> errFiles = {
        { 9, 3},
        {10, 2},
        {11, 2},
        {13, 3},
        {14, 12},
        {15, 3},
        {16, 7},
        {17, 5},
        {18, 5},
        {19, 8},
        {20, 3},
        {21, 5},
        {22, 7},
        {23, 7},
        {24, 5},
        {25, 5},
        {26, 3},
        {28, 7},
        {29, 7},
        {31, 3},
        {32, 6},
        {33, 3},
        {34, 5},
        {35, 5},
        {36, 5},
        {40, 3},
        {45, 5}
    }; 

    for (int i = 1; i <= 48; i++) {
        char filename[26];
        sprintf(filename, "tiger-programs/test%d.tig", i);
        yyin = fopen(filename, "r");
        auto b = buffman::Buffman(yyin);
        if (errFiles.find(i) == errFiles.end()){
            SECTION("semantics for test" + std::to_string(i) + ".tig") {
                REQUIRE(yyparse() == 0);
                REQUIRE(semantic_checks(programNode.get()) == 0);
            }
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
