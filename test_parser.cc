#define CATCH_CONFIG_MAIN
#include <string.h>
#include "catch.hpp"
#include "ast.hh"
#include "tiger.tab.h"
#include "buffman.hh"

extern FILE* yyin;
extern std::shared_ptr<tiger::ASTNode> programNode;

namespace tiger {


TEST_CASE("placeholder test","[syntax]") {
    REQUIRE(true);
}

TEST_CASE("test that nil gets parsed","[syntax]") {
    auto b = Buffman("nil");
    yyparse();
    REQUIRE(programNode->toStr() == "program");
}

TEST_CASE("test that variables get parsed correctly","[syntax]") {
    auto b = Buffman("var myid := 4");
    REQUIRE(yyparse() == VAR);
}

} //namespace
