#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "ast.hh" 
#include "tiger.tab.h"
#include "buffman.hh"

#ifndef YY_BUF_SIZE
#define YY_BUF_SIZE 16384
#endif

extern int yylex(void);
extern FILE* yyin;
extern YY_BUFFER_STATE yy_scan_string(const char*);
extern YY_BUFFER_STATE yy_create_buffer(FILE*,int);
extern void yy_switch_to_buffer(YY_BUFFER_STATE);
extern void yy_delete_buffer(YY_BUFFER_STATE);

TEST_CASE("Buffman lexing tests","[buffman]") {
    SECTION("buffman string constructor works") {
        auto b = buffman::Buffman("if");
        REQUIRE(yylex() == IF);
    }
    SECTION("buffman file constructor works") {
        yyin = fopen("tiger-programs/test2.tig", "r");
        auto b = buffman::Buffman(yyin);
        REQUIRE(yylex() == LET);
    }
}

const std::vector<int> tEnm = {ARRAY,IF,THEN,
                                ELSE,WHILE,FOR,TO,
                                DO,LET,IN,END,OF,
                                BREAK,NIL,FUNCTION,
                                VAR,TYPE,IMPORT,PRIMITIVE,
                                CLASS,EXTENDS,METHOD,NEW,
                                NOTEQUAL,ELESS,EGREATER,
                                ASSIGNMENT,STRINGLIT,INTLIT,
                                ID,ERROR};

const std::vector<std::string>  tStr = {"array","if","then",
                          "else","while","for","to",
                          "do","let","in","end","of",
                          "break","nil","function",
                          "var","type","import","primitive",
                          "class","extends","method","new",
                          "<>","<=",">=",
                          ":=","\"foo\"","239",
                          "id", "!err"};

/*Loops through parallel arrays tStr and tEnm, feeding
in a test string from tStr with the desired output at the same
index in tEnm*/
TEST_CASE("Basic Test Case for Keywords","[tokens]") {

	YY_BUFFER_STATE testBuffer;

	for(auto i=0; i < static_cast<int>(tStr.size());++i){
		SECTION("Plain test for enum #" + std::to_string(i) + " and input " + tStr[i]){
			testBuffer = yy_scan_string(tStr[i].c_str());
			yy_switch_to_buffer(testBuffer);
			REQUIRE(yylex() == tEnm[i]);
			yy_delete_buffer(testBuffer);
		}
	}

}

const std::vector<char> tEnmC = {',',':',';',
                                '(',')','[',']', '{','}','.','+','-','*',
                                '/','=','<','>','&','|'};

const std::vector<std::string> tStrC = {",",":",";","(",")","[","]", "{","}",".","+","-","*","/","=","<",">","&","|"};

TEST_CASE("Basic Test Case for Chars","[tokens]") {

	YY_BUFFER_STATE testBuffer;

	for(auto i=0; i < static_cast<int>(tStrC.size());++i){
		SECTION("Plain test for enum #" + std::to_string(i) + " and input " + tStrC[i]){
			testBuffer = yy_scan_string(tStrC[i].c_str());
			yy_switch_to_buffer(testBuffer);
			REQUIRE(yylex() == tEnmC[i]);
			yy_delete_buffer(testBuffer);
		}
	}

}

/*Makes sure everything in the list tStrlit is a string literal
First part of the list is ascii characters 32-126
Then its newline characters
Then its other supported escapes
Then its a long string
*/
TEST_CASE("More string literal tests","[tokens]") {

	const std::vector<std::string>  tStrlit = {"\" \"","\"!\"","\"\\\"\"","\"#\"","\"$\"",
                "\"%\"","\"&\"","\"'\"","\"(\"","\")\"","\"*\"","\"+\"","\",\"","\"-\"",
                "\".\"","\"/\"","\"0\"","\"1\"","\"2\"","\"3\"","\"4\"","\"5\"","\"6\"",
                "\"7\"","\"8\"","\"9\"","\":\"","\";\"","\"<\"","\"=\"","\">\"","\"?\"",
                "\"@\"","\"A\"","\"B\"","\"C\"","\"D\"","\"E\"","\"F\"","\"G\"","\"H\"",
                "\"I\"","\"J\"","\"K\"","\"L\"","\"M\"","\"N\"","\"O\"","\"P\"","\"Q\"",
                "\"R\"","\"S\"","\"T\"","\"U\"","\"V\"","\"W\"","\"X\"","\"Y\"","\"Z\"",
                "\"[\"","\"\\\\\"","\"]\"","\"^\"","\"_\"","\"`\"","\"a\"","\"b\"","\"c\"",
                "\"d\"","\"e\"","\"f\"","\"g\"","\"h\"","\"i\"","\"j\"","\"k\"","\"l\"",
                "\"m\"","\"n\"","\"o\"","\"p\"","\"q\"","\"r\"","\"s\"","\"t\"","\"u\"",
                "\"v\"","\"w\"","\"x\"","\"y\"","\"z\"","\"{\"","\"|\"","\"}\"","\"~\"",
                "\"     \"","\"\\n\"","\"\\r\"","\"\\n\\r\"","\"\\r\\n\"",
                "\"\\a\"", "\"\\b\"", "\"\\f\"", "\"\\t\"", "\"\\v\"",
                "\"\\x0a\"", "\"\\x1f\"", "\"\\x6e\"", "\"\\b3\"", "\"\\xc4\"",
                "\"\\377\"", "\"\\001\"", "\"\\000\"", "\"\\040\"", "\"\\225\"",
                "\"longer string of stuff\"", "\"blahalal\\a\"", "\"\\ajdfnjdfj\""};

	YY_BUFFER_STATE testBuffer;

	for(auto i=0; i < static_cast<int>(tStrlit.size());++i){
		SECTION("Plain test for string " + tStrlit[i]){
			testBuffer = yy_scan_string(tStrlit[i].c_str());
			yy_switch_to_buffer(testBuffer);
			REQUIRE(yylex() == STRINGLIT);
			yy_delete_buffer(testBuffer);
		}
	}

}
/*Makes sure everything in the list tInt is an Integer literal
*/
TEST_CASE("More integer tests","[tokens]") {

	const std::vector<std::string>  tInt = {"0","1","2","3","4","5","6","7","8","9"};

	YY_BUFFER_STATE testBuffer;

	for(auto i=0; i < static_cast<int>(tInt.size());++i){
		SECTION("Plain test for integer " + tInt[i]){
			testBuffer = yy_scan_string(tInt[i].c_str());
			yy_switch_to_buffer(testBuffer);
			REQUIRE(yylex() == INTLIT);
			yy_delete_buffer(testBuffer);
		}
	}

}
/*Makes sure everything in the list tId is an identifier
*/
TEST_CASE("More identifier tests","[tokens]") {

	const std::vector<std::string>  tId = {"myid", "m2389478", "y___203940923_efljg_fjgnjk", "_main"};

	YY_BUFFER_STATE testBuffer;

	for(auto i=0; i < static_cast<int>(tId.size());++i){
		SECTION("Plain test for identifier " + tId[i]){
			testBuffer = yy_scan_string(tId[i].c_str());
			yy_switch_to_buffer(testBuffer);
			REQUIRE(yylex() == ID);
			yy_delete_buffer(testBuffer);
		}
	}

}
/*Makes sure everything in the list tErr returns the ERROR token
*/
TEST_CASE("make sure some strings throw errors","[tokens]") {

	const std::vector<std::string>  tErr = {"!", "_man", "_mayn", "_anythingelse",
        /* bad string escapes */                "\"\\c\"", "\"\\d\"", "\"\\e\"", "\"\\g\"", "\"\\z\"",
                                                "\"\\x\"", "\"\\y\"", "\"\\i\"", "\"\\h\"", "\"\\\"",
                                                "\"\\x0\"", "\"\\xd\"", "\"\\x0h\"", "\"\\xg2\"",
                                                "\"\\378\"", "\"\\400\"", "\"\\008\"", "\"\\099\"",
                                                "\"", "\"whoopsie I forgot to terminate my string"};

	YY_BUFFER_STATE testBuffer;

	for(auto i=0; i < static_cast<int>(tErr.size());++i){
		SECTION("Plain test for identifier " + tErr[i]){
			testBuffer = yy_scan_string(tErr[i].c_str());
			yy_switch_to_buffer(testBuffer);
			REQUIRE(yylex() == ERROR);
			yy_delete_buffer(testBuffer);
		}
	}

}

/* These next few tests will be reading from files
All of these files are from:
https://www.cs.princeton.edu/~appel/modern/testcases/
*/
TEST_CASE("make sure real file test1.tig works","[tokens]") {
    yyin = fopen("tiger-programs/test1.tig", "r");
    YY_BUFFER_STATE testBuffer = yy_create_buffer(yyin,YY_BUF_SIZE);
    yy_switch_to_buffer(testBuffer);
    REQUIRE(yylex() == LET);
    REQUIRE(yylex() == TYPE);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == '=');
    REQUIRE(yylex() == ARRAY);
    REQUIRE(yylex() == OF);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == VAR);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == ':');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == ASSIGNMENT);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == '[');
    REQUIRE(yylex() == INTLIT);
    REQUIRE(yylex() == ']');
    REQUIRE(yylex() == OF);
    REQUIRE(yylex() == INTLIT);
    REQUIRE(yylex() == IN);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == END);
    REQUIRE(yylex() == 0); //yylex()outputs 0 when nothing lexed
    yy_delete_buffer(testBuffer);
    fclose(yyin);
}

TEST_CASE("make sure real file test2.tig works","[tokens]") {
    yyin = fopen("tiger-programs/test2.tig", "r");
    YY_BUFFER_STATE testBuffer = yy_create_buffer(yyin,YY_BUF_SIZE);
    yy_switch_to_buffer(testBuffer);
    REQUIRE(yylex() == LET);
    REQUIRE(yylex() == TYPE);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == '=');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == TYPE);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == '=');
    REQUIRE(yylex() == ARRAY);
    REQUIRE(yylex() == OF);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == VAR);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == ':');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == ASSIGNMENT);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == '[');
    REQUIRE(yylex() == INTLIT);
    REQUIRE(yylex() == ']');
    REQUIRE(yylex() == OF);
    REQUIRE(yylex() == INTLIT);
    REQUIRE(yylex() == IN);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == END);
    REQUIRE(yylex() == 0); //yylex()outputs 0 when nothing lexed
    yy_delete_buffer(testBuffer);
    fclose(yyin);
}

TEST_CASE("make sure test3.tig works","[tokens]") {
    yyin = fopen("tiger-programs/test3.tig", "r");
    YY_BUFFER_STATE testBuffer = yy_create_buffer(yyin,YY_BUF_SIZE);
    yy_switch_to_buffer(testBuffer);
    REQUIRE(yylex() == LET);
    REQUIRE(yylex() == TYPE);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == '=');
    REQUIRE(yylex() == '{');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == ':');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == ',');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == ':');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == '}');
    REQUIRE(yylex() == VAR);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == ':');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == ASSIGNMENT);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == '{');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == '=');
    REQUIRE(yylex() == STRINGLIT);
    REQUIRE(yylex() == ',');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == '=');
    REQUIRE(yylex() == INTLIT);
    REQUIRE(yylex() == '}');
    REQUIRE(yylex() == IN);
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == '.');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == ASSIGNMENT);
    REQUIRE(yylex() == STRINGLIT);
    REQUIRE(yylex() == ';');
    REQUIRE(yylex() == ID);
    REQUIRE(yylex() == END);
    REQUIRE(yylex() == 0); //yylex()outputs 0 when nothing lexed
    yy_delete_buffer(testBuffer);
    fclose(yyin);
}

TEST_CASE("make sure commentTest.tig works","[tokens]") {
    SECTION("Test with a file of just comments"){
        yyin = fopen("tiger-programs/commentTest.tig", "r");
        YY_BUFFER_STATE testBuffer = yy_create_buffer(yyin,YY_BUF_SIZE);
        yy_switch_to_buffer(testBuffer);
        REQUIRE(yylex() == 0); //yylex()outputs 0 when nothing lexed
        yy_delete_buffer(testBuffer);
        fclose(yyin);
    }
    SECTION("Test with a file of comments and an array keywords"){
        yyin = fopen("tiger-programs/commentTest_ARRAY.tig", "r");
        YY_BUFFER_STATE testBuffer = yy_create_buffer(yyin,YY_BUF_SIZE);
        yy_switch_to_buffer(testBuffer);
        REQUIRE(yylex() == ARRAY);
        REQUIRE(yylex() == 0); //yylex()outputs 0 when nothing lexed
        yy_delete_buffer(testBuffer);
        fclose(yyin);
    }
}
TEST_CASE("make sure newline in string errors","[tokens]") {
    yyin = fopen("tiger-programs/newline_in_string.tig", "r");
    YY_BUFFER_STATE testBuffer = yy_create_buffer(yyin,YY_BUF_SIZE);
    yy_switch_to_buffer(testBuffer);
    REQUIRE(yylex() == ERROR);
    //REQUIRE(yylex() == 0); //yylex()outputs 0 when nothing lexed
    yy_delete_buffer(testBuffer);
    fclose(yyin);
}
