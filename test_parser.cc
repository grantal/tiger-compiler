#define CATCH_CONFIG_MAIN
#include <string.h>
#include "catch.hpp"
#include "ast.hh"
#include "tiger.tab.h"
#include "buffman.hh"

extern FILE* yyin;

TEST_CASE("placeholder test","[syntax]") {
    REQUIRE(true);
}

TEST_CASE("test that nil gets parsed","[syntax]") {
    auto b = Buffman("nil");
    REQUIRE(yyparse() == 23);
}

TEST_CASE("test that variables get parsed correctly","[syntax]") {
    auto b = Buffman("var myid := 4");
    REQUIRE(yyparse() == VAR);
}
