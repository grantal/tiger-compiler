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

TEST_CASE("Test structure of some satements", "[syntax]") {

    SECTION("array creation") {
            auto b = buffman::Buffman("id [1] of 2");
            REQUIRE(yyparse() == 0); 
            ParentASTNode* arrayCreate = (ParentASTNode*)programNode->_getChild(0);
            // make sure ID child is good
            REQUIRE(arrayCreate->_getChild(0)->toStr().find("id") != std::string::npos);
            REQUIRE(arrayCreate->_getChild(0)->toStr().find("1") == std::string::npos);
            REQUIRE(arrayCreate->_getChild(0)->toStr().find("2") == std::string::npos);
            // make sure length child is good
            REQUIRE(arrayCreate->_getChild(1)->toStr().find("id") == std::string::npos);
            REQUIRE(arrayCreate->_getChild(1)->toStr().find("1") != std::string::npos);
            REQUIRE(arrayCreate->_getChild(1)->toStr().find("2") == std::string::npos);
            // make sure initializer child is good
            REQUIRE(arrayCreate->_getChild(2)->toStr().find("id") == std::string::npos);
            REQUIRE(arrayCreate->_getChild(2)->toStr().find("1") == std::string::npos);
            REQUIRE(arrayCreate->_getChild(2)->toStr().find("2") != std::string::npos);
    } 

    SECTION("object creation") {
            auto b = buffman::Buffman("new id");
            REQUIRE(yyparse() == 0); 
            ParentASTNode* objectCreate = (ParentASTNode*)programNode->_getChild(0);
            REQUIRE(objectCreate->toStr().find("new") != std::string::npos);
            REQUIRE(objectCreate->_getChild(0)->toStr().find("id") != std::string::npos);
            REQUIRE(objectCreate->_getChild(0)->toStr().find("new") == std::string::npos);
    } 
    SECTION("function call") {
            auto b = buffman::Buffman("id(1)");
            REQUIRE(yyparse() == 0);
            ParentASTNode* funCall = (ParentASTNode*)programNode->_getChild(0);
            REQUIRE(funCall->toStr().find("function") != std::string::npos);
            // check function name
            REQUIRE(funCall->_getChild(0)->toStr().find("id") != std::string::npos);
            REQUIRE(funCall->_getChild(0)->toStr().find("1") == std::string::npos);
            // check function args
            REQUIRE(funCall->_getChild(1)->toStr().find("1") != std::string::npos);
            REQUIRE(funCall->_getChild(1)->toStr().find("id") == std::string::npos);
    } 
    SECTION("negative") {
            auto b = buffman::Buffman("-1");
            REQUIRE(yyparse() == 0);
            ParentASTNode* mainNode = (ParentASTNode*)programNode->_getChild(0);
            REQUIRE(mainNode->toStr().find("-") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("1") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("-") == std::string::npos);
    }
}

TEST_CASE("Test Binary Ops structure", "[syntax]") {
    const std::vector<std::string> binOps = {
        "+", "-", "*", "/", "=", "<>", ">", "<", ">=", "<=", "&", "|" 
    };
    for(auto i=0; i < static_cast<int>(binOps.size());++i){
        SECTION("Test for bin op \"" + binOps[i] +"\""){
            auto b = buffman::Buffman("1" + binOps[i] + "2");
            REQUIRE(yyparse() == 0);
            ParentASTNode* mainNode = (ParentASTNode*)programNode->_getChild(0);
            REQUIRE(mainNode->toStr().find(binOps[i]) != std::string::npos);
            // make sure left operand is good
            REQUIRE(mainNode->_getChild(0)->toStr().find("1") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("2") == std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find(binOps[i]) == std::string::npos);
            // make sure right operand is good
            REQUIRE(mainNode->_getChild(1)->toStr().find("2") != std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("1") == std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find(binOps[i]) == std::string::npos);
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
