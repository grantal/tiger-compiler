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
        "nil", "id", "32", "\"string\"", "let", 
        "+", "-", "*", "/", "=", "<>", "<", ">",
        "<=", ">=", "&", "|", "assignment", "if",
        "else", "while", "for", "break",
        "type", "class", "var", "function", 
        "primitive", "import" 
    };

    const std::vector<std::string> statements = {
        "nil", "id", "32", "\"string\"", "let in end",
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
            auto b = buffman::Buffman("id [\"length\"] of \"init\"");
            REQUIRE(yyparse() == 0); 
            ParentASTNode* arrayCreate = (ParentASTNode*)programNode->_getChild(0);
            // make sure ID child is good
            REQUIRE(arrayCreate->_getChild(0)->toStr().find("id") != std::string::npos);
            REQUIRE(arrayCreate->_getChild(0)->toStr().find("length") == std::string::npos);
            REQUIRE(arrayCreate->_getChild(0)->toStr().find("init") == std::string::npos);
            // make sure length child is good
            REQUIRE(arrayCreate->_getChild(1)->toStr().find("id") == std::string::npos);
            REQUIRE(arrayCreate->_getChild(1)->toStr().find("length") != std::string::npos);
            REQUIRE(arrayCreate->_getChild(1)->toStr().find("init") == std::string::npos);
            // make sure initializer child is good
            REQUIRE(arrayCreate->_getChild(2)->toStr().find("id") == std::string::npos);
            REQUIRE(arrayCreate->_getChild(2)->toStr().find("length") == std::string::npos);
            REQUIRE(arrayCreate->_getChild(2)->toStr().find("init") != std::string::npos);
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
            REQUIRE(mainNode->getNodeType() == nodeType::NEGATE);
            REQUIRE(mainNode->_getChild(0)->toStr().find("1") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("-") == std::string::npos);
    }
    SECTION("assignment") {
            auto b = buffman::Buffman("id := 1");
            REQUIRE(yyparse() == 0);
            ParentASTNode* mainNode = (ParentASTNode*)programNode->_getChild(0);
            // id should be first
            REQUIRE(mainNode->_getChild(0)->toStr().find("id") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("1") == std::string::npos);
            // number should be second
            REQUIRE(mainNode->_getChild(1)->toStr().find("1") != std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("id") == std::string::npos);
    }
    SECTION("if then") {
            auto b = buffman::Buffman("if \"cond\" then \"action\"");
            REQUIRE(yyparse() == 0);
            ParentASTNode* mainNode = (ParentASTNode*)programNode->_getChild(0);
            // condition
            REQUIRE(mainNode->_getChild(0)->toStr().find("cond") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("action") == std::string::npos);
            // action
            REQUIRE(mainNode->_getChild(1)->toStr().find("action") != std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("cond") == std::string::npos);
    }
    SECTION("if then else") {
            auto b = buffman::Buffman("if \"cond\" then \"action\" else \"other\"");
            REQUIRE(yyparse() == 0);
            ParentASTNode* mainNode = (ParentASTNode*)programNode->_getChild(0);
            // condition
            REQUIRE(mainNode->_getChild(0)->toStr().find("cond") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("action") == std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("other") == std::string::npos);
            // action
            REQUIRE(mainNode->_getChild(1)->toStr().find("action") != std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("cond") == std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("other") == std::string::npos);
            // else
            REQUIRE(mainNode->_getChild(2)->toStr().find("other") != std::string::npos);
            REQUIRE(mainNode->_getChild(2)->toStr().find("cond") == std::string::npos);
            REQUIRE(mainNode->_getChild(2)->toStr().find("action") == std::string::npos);
    }
    SECTION("while loop") {
            auto b = buffman::Buffman("while \"cond\" do \"action\"");
            REQUIRE(yyparse() == 0);
            ParentASTNode* mainNode = (ParentASTNode*)programNode->_getChild(0);
            // condition
            REQUIRE(mainNode->_getChild(0)->toStr().find("cond") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("action") == std::string::npos);
            // action
            REQUIRE(mainNode->_getChild(1)->toStr().find("action") != std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("cond") == std::string::npos);
    }
    SECTION("break") {
            auto b = buffman::Buffman("while 1 do break");
            REQUIRE(yyparse() == 0);
            ParentASTNode* mainNode = (ParentASTNode*)programNode->_getChild(0);
            REQUIRE(mainNode->_getChild(1)->toStr().find("break") != std::string::npos);
    }
    SECTION("for loop") {
            auto b = buffman::Buffman("for iter := \"start\" to \"end\" do \"action\"");
            REQUIRE(yyparse() == 0);
            ParentASTNode* mainNode = (ParentASTNode*)programNode->_getChild(0);
            // iterator variable
            REQUIRE(mainNode->_getChild(0)->toStr().find("iter") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("start") == std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("end") == std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("action") == std::string::npos);
            // start number
            REQUIRE(mainNode->_getChild(1)->toStr().find("start") != std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("end") == std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("action") == std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("iter") == std::string::npos);
            // end number
            REQUIRE(mainNode->_getChild(2)->toStr().find("end") != std::string::npos);
            REQUIRE(mainNode->_getChild(2)->toStr().find("start") == std::string::npos);
            REQUIRE(mainNode->_getChild(2)->toStr().find("action") == std::string::npos);
            REQUIRE(mainNode->_getChild(2)->toStr().find("iter") == std::string::npos);
            // action
            REQUIRE(mainNode->_getChild(3)->toStr().find("action") != std::string::npos);
            REQUIRE(mainNode->_getChild(3)->toStr().find("start") == std::string::npos);
            REQUIRE(mainNode->_getChild(3)->toStr().find("end") == std::string::npos);
            REQUIRE(mainNode->_getChild(3)->toStr().find("iter") == std::string::npos);

    }
    SECTION("let/in") {
            auto b = buffman::Buffman("let var x := 1 in 3 end");
            REQUIRE(yyparse() == 0);
            ParentASTNode* mainNode = (ParentASTNode*)programNode->_getChild(0);
            // condition
            REQUIRE(mainNode->_getChild(0)->toStr().find("x") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("1") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("3") == std::string::npos);
            // action
            REQUIRE(mainNode->_getChild(1)->toStr().find("3") != std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("1") == std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("x") == std::string::npos);
    }
}

TEST_CASE("Test Binary Ops structure", "[syntax]") {
    const std::vector<std::string> binOps = {
        "+", "-", "*", "/", "=", "<>", ">", "<", ">=", "<=", "&", "|" 
    };
    for(auto i=0; i < static_cast<int>(binOps.size());++i){
        SECTION("Test for bin op \"" + binOps[i] +"\""){
            auto b = buffman::Buffman("\"operand1\"" + binOps[i] + "\"operand2\"");
            REQUIRE(yyparse() == 0);
            ParentASTNode* mainNode = (ParentASTNode*)programNode->_getChild(0);
            REQUIRE(mainNode->toStr().find(binOps[i]) != std::string::npos);
            // make sure left operand is good
            REQUIRE(mainNode->_getChild(0)->toStr().find("operand1") != std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find("operand2") == std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find(binOps[i]) == std::string::npos);
            // make sure right operand is good
            REQUIRE(mainNode->_getChild(1)->toStr().find("operand2") != std::string::npos);
            REQUIRE(mainNode->_getChild(1)->toStr().find("operand1") == std::string::npos);
            REQUIRE(mainNode->_getChild(0)->toStr().find(binOps[i]) == std::string::npos);
        }
    }
    
}
    
/* These next few tests will be reading from files
Some of these files are from:
https://www.cs.princeton.edu/~appel/modern/testcases/
*/
TEST_CASE("make sure real file test1.tig parses correctly","[syntax]") {
    SECTION("type arrtype = array of int") {
        auto b = buffman::Buffman("type arrtype = array of int");
        REQUIRE(yyparse() == 0);
    }
    SECTION("var arr1:arrtype := arrtype [10] of 0") {
        auto b = buffman::Buffman("var arr1:arrtype := arrtype [10] of 0");
        REQUIRE(yyparse() == 0);
    }
    SECTION("arr1") {
        auto b = buffman::Buffman("arr1");
        REQUIRE(yyparse() == 0);
    }
    yyin = fopen("tiger-programs/test1.tig", "r");
    auto b = buffman::Buffman(yyin);
    REQUIRE(yyparse() == 0);
}
TEST_CASE("make sure newline_in_string.tig errors","[syntax]") {
    std::stringstream buffer;
    std::streambuf * old = std::cerr.rdbuf(buffer.rdbuf());
    yyin = fopen("tiger-programs/newline_in_string.tig", "r");
    auto b = buffman::Buffman(yyin);
    REQUIRE(yyparse() == 1);
    std::cerr.rdbuf(old);
}
TEST_CASE("throw errors correctly, file errorTest.tig","[syntax]") {
    std::stringstream buffer;
    std::streambuf * old = std::cerr.rdbuf(buffer.rdbuf());
    yyin = fopen("tiger-programs/errorTest.tig", "r");
    auto b = buffman::Buffman(yyin);
    REQUIRE(yyparse() == 1);
    // error should be on line 6
    REQUIRE(buffer.str().find("6") != std::string::npos);
    std::cerr.rdbuf(old);
}

TEST_CASE("parses long lval chaings, file lValTest.tig","[syntax]") {
    yyin = fopen("tiger-programs/lValTest.tig", "r");
    auto b = buffman::Buffman(yyin);
    REQUIRE(yyparse() == 0);
    std::cout << programNode->toStr() << std::endl;
}

TEST_CASE("test that all of appel's testfiles that should parse, do parse","[syntax]") {
    for (int i = 1; i <= 48; i++) {
        SECTION("parse test" + std::to_string(i) + ".tig") {
            char filename[26];
            sprintf(filename, "tiger-programs/test%d.tig", i);
            yyin = fopen(filename, "r");
            auto b = buffman::Buffman(yyin);
            REQUIRE(yyparse() == 0);
        }
    }
}
TEST_CASE("Another error message test, of test49.tig","[syntax]") {
    std::stringstream buffer;
    std::streambuf * old = std::cerr.rdbuf(buffer.rdbuf());
    yyin = fopen("tiger-programs/test49.tig", "r");
    auto b = buffman::Buffman(yyin);
    REQUIRE(yyparse() == 1);
    // error should be on line 5
    REQUIRE(buffer.str().find("5") != std::string::npos);
    std::cerr.rdbuf(old);
}

TEST_CASE("test references", "[syntax]"){
    SECTION("standard references") {
            auto buff = buffman::Buffman("a.b.c.d");
            REQUIRE(yyparse() == 0);
            // right child of ETCd should be d, left should be another lval with a,b,c as children
            ParentASTNode* ETCd = (ParentASTNode*)programNode->_getChild(0);
            REQUIRE(ETCd->getNodeType() == nodeType::REFERENCE);
            TokenASTNode* d = (TokenASTNode*)ETCd->_getChild(1);
            REQUIRE(d->getToken() == ID);
            REQUIRE(d->getVal() == "d");
            // right should be c, left should be a and b
            ParentASTNode* ETCc = (ParentASTNode*)ETCd->_getChild(0);
            REQUIRE(ETCc->getNodeType() == nodeType::REFERENCE);
            TokenASTNode* c = (TokenASTNode*)ETCc->_getChild(1);
            REQUIRE(c->getToken() == ID);
            REQUIRE(c->getVal() == "c");
            // left should be a, right should be b 
            ParentASTNode* aANDb = (ParentASTNode*)ETCc->_getChild(0);
            REQUIRE(aANDb->getNodeType() == nodeType::REFERENCE);
            TokenASTNode* a = (TokenASTNode*)aANDb->_getChild(0);
            TokenASTNode* b = (TokenASTNode*)aANDb->_getChild(1);
            REQUIRE(a->getToken() == ID);
            REQUIRE(a->getVal() == "a");
            REQUIRE(b->getToken() == ID);
            REQUIRE(b->getVal() == "b");
    }
    SECTION("array references") {
            auto buff = buffman::Buffman("a[1][2][3][4]");
            REQUIRE(yyparse() == 0);
            // left of a1234 should be another array reference with a,1,2,3 as children/grandchildren
            // right should be 4
            ParentASTNode* a1234 = (ParentASTNode*)programNode->_getChild(0);
            REQUIRE(a1234->getNodeType() == nodeType::ARRAY_REF);
            TokenASTNode* four = (TokenASTNode*)a1234->_getChild(1);
            REQUIRE(four->getToken() == INTLIT);
            REQUIRE(four->getVal() == "4");
            // left of a123 is a12, right is 3
            ParentASTNode* a123 = (ParentASTNode*)a1234->_getChild(0);
            REQUIRE(a123->getNodeType() == nodeType::ARRAY_REF);
            TokenASTNode* three = (TokenASTNode*)a123->_getChild(1);
            REQUIRE(three->getToken() == INTLIT);
            REQUIRE(three->getVal() == "3");
            // left should be a and 1, right is 2
            ParentASTNode* a12 = (ParentASTNode*)a123->_getChild(0);
            REQUIRE(a12->getNodeType() == nodeType::ARRAY_REF);
            TokenASTNode* two = (TokenASTNode*)a12->_getChild(1);
            REQUIRE(two->getToken() == INTLIT);
            REQUIRE(two->getVal() == "2");
            // left should be a, right should be 1
            ParentASTNode* a1 = (ParentASTNode*)a12->_getChild(0);
            REQUIRE(a1->getNodeType() == nodeType::ARRAY_REF);
            TokenASTNode* a = (TokenASTNode*)a1->_getChild(0);
            TokenASTNode* one = (TokenASTNode*)a1->_getChild(1);
            REQUIRE(a->getToken() == ID);
            REQUIRE(a->getVal() == "a");
            REQUIRE(one->getToken() == INTLIT);
            REQUIRE(one->getVal() == "1");
    }
}

} //namespace
