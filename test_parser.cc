#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "tiger.tab.h"

extern "C"{
	extern int yylex(void);
}

TEST_CASE("placeholder test","[syntax]") {
    REQUIRE(true);
}
