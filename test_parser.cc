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



TEST_CASE("Test Keywords shows up in ast", "[syntax]") {
    const std::vector<std::string> keywords = {
        "nil", "id", "32", "\"string\"", "new", "let", 
        "+", "-", "*", "/", "=", "<>", "<", ">",
        "<=", ">=", "&", "|", "assignment", "if",
        "else", "while", "for", "break",
        "type", "class", "var", "function", 
        "primitive", "import" 
    };

    const std::vector<std::string> statements = {
        "nil", "id", "32", "\"string\"", "new id", "let in end"
        "1+1", "1-1", "1*1", "1/1", "1=1", "1<>1", "1<1", "1>1",
        "1<=1", "1>=1", "1&1", "1|1", "id := 2", "if 1 then 2",
        "if 1 then 2 else 3", "while 4 do 5", "for i := 1 to 2 do 3", "break",
        "type id = id", "class id {}", "var id := 1", "function id () = 1", 
        "primitive id ()", "import \"string\"" 
    };
    for(auto i=0; i < static_cast<int>(statements.size());++i){
        SECTION("Test that keyword \"" + keywords[i] + "\" is in the AST for statement \"" + statements[i] + "\""){
            auto b = buffman::Buffman(statements[i]);
            REQUIRE(yyparse() == 0); //zero means it parsed successfully
            REQUIRE(programNode->toStr().find(keywords[i]) != std::string::npos);
        }
    }
}
    
/* These next few tests will be reading from files
All of these files are from:
https://www.cs.princeton.edu/~appel/modern/testcases/
*/
TEST_CASE("make sure real file test1.tig parses correctly","[syntax]") {
    yyin = fopen("tiger-programs/test1.tig", "r");
    auto b = buffman::Buffman(yyin);
    REQUIRE(yyparse() == 0);
    fclose(yyin);
}

} //namespace
