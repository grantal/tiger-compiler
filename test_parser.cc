#define CATCH_CONFIG_MAIN
#include <string>
#include <memory>
#include "catch.hpp"
#include "ast.hh"
#include "tiger.tab.h"
#include "buffman.hh"

extern FILE* yyin;

namespace tiger {
extern std::shared_ptr<ASTNode> programNode;

TEST_CASE("test that nil gets parsed","[syntax]") {
    auto b = buffman::Buffman("nil");
    REQUIRE(yyparse() == 0); //zero means it parsed successfully
    // check that the word "nil" appears in the toStr
    REQUIRE(programNode->toStr().find("nil") != std::string::npos);
}

TEST_CASE("test that intlits get parsed","[syntax]") {
    auto b = buffman::Buffman("32");
    REQUIRE(yyparse() == 0); //zero means it parsed successfully
    // check that the word "nil" appears in the toStr
    REQUIRE(programNode->toStr().find("32") != std::string::npos);
}

} //namespace
