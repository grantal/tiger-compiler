#define CATCH_CONFIG_MAIN
#include <string.h>
#include "catch.hpp"
#include "tiger.tab.h"
#include "token.hh" //TODO: remove once all tokens are defined in tiger.tab.h
#include "buffman.hh"

extern FILE* yyin;

TEST_CASE("placeholder test","[syntax]") {
    REQUIRE(true);
}

TEST_CASE("test that variables get parsed correctly","[syntax]") {
    YY_BUFFER_STATE testBuffer;
    testBuffer = yy_scan_string("var myid := 4");
    yy_switch_to_buffer(testBuffer);
    REQUIRE(yyparse() == INTLIT);
    yy_delete_buffer(testBuffer);
}
