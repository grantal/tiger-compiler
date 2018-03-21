#define CATCH_CONFIG_MAIN
#include <string>
#include <memory>
#include "catch.hpp"
#include "ast.hh"
#include "tiger.tab.h"
#include "buffman.hh"

extern FILE* yyin;

namespace tiger {
extern std::shared_ptr<ParentASTNode> programNode;
extern int semantic_checks(ASTNode::ASTptr);

TEST_CASE("test that nil gets parsed","[syntax]") {
    auto b = buffman::Buffman("nil");
    REQUIRE(yyparse() == 0);
    REQUIRE(semantic_checks(programNode.get()) == 0);
}

} //namespace
