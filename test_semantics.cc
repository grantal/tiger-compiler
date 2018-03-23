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
    for (int i = 1; i <= 48; i++) {
        SECTION("semantics for test" + std::to_string(i) + ".tig") {
            char filename[26];
            sprintf(filename, "tiger-programs/test%d.tig", i);
            yyin = fopen(filename, "r");
            auto b = buffman::Buffman(yyin);
            REQUIRE(yyparse() == 0);
            REQUIRE(semantic_checks(programNode.get()) == 0);
        }
    }
}

} //namespace
