#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "token.hh"

extern "C"{
	extern int yylex(void);
	typedef struct yy_buffer_state* YY_BUFFER_STATE;
	extern YY_BUFFER_STATE yy_scan_string(const char*);
	extern void yy_switch_to_buffer(YY_BUFFER_STATE);
	extern void yy_delete_buffer(YY_BUFFER_STATE);
	extern FILE* yyin;
}

	const std::vector<int> tEnm = {ARRAY,IF,THEN,
					ELSE,WHILE,FOR,TO,
					DO,LET,IN,END,OF,
					BREAK,NIL,FUNCTION,
					VAR,TYPE,IMPORT,PRIMITIVE,
					CLASS,EXTENDS,METHODS,NEW,
                                        COMMA,COLON,SEMICOLON,
                                        LPAREN,RPAREN,LBRACKET,RBRACKET,
                                        LBRACE,RBRACE,DOT,PLUS,MINUS,STAR,
                                        FSLASH,EQUAL,NOTEQUAL,LESS,GREATER,
                                        ELESS,EGREATER,AND,OR,
                                        ASSIGNMENT,ENDL,STRINGLIT,INTLIT,
                                        IDENTIFIER,ERROR};

	const std::vector<std::string>  tStr = {"array","if","then",
				  "else","while","for","to",
				  "do","let","in","end","of",
				  "break","nil","function",
				  "var","type","import","primitive",
				  "class","extends","methods","new",
				  ",",":",";",
                                  "(",")","[","]",
                                  "{","}",".","+","-","*",
                                  "/","==","<>","<",">",
                                  "=>","=<","&","|"
				  ":=","\r","\"foo\"","239",
                                  "id", "!err"};

/*
        const std::vector<std::string> tGarbage = {"array","if","then",
                                  "else","while","for","to",
                                  "do","let","in","end","of",
                                  "break","nil","function",
                                  "var","type","import","primitive",
                                  "class","extends","methods","new",
                                  ",",":",";","(",")","[","]",".","+",
                                  "-","*","/","==","<>","<",">","=>","=<",
                                  "=","\"foo\"","239"," ","\t","\n"};
*/

/*Loops through parallel arrays tStr and tEnm, feeding
in a test string from tStr with the desired output at the same
index in tEnm*/
TEST_CASE("Basic Test Case for Keywords","[tokens]") {

	YY_BUFFER_STATE testBuffer;

	for(auto i=0; i < static_cast<int>(tStr.size());++i){
		SECTION("Plain test for enum #" + std::to_string(i)){
			testBuffer = yy_scan_string(tStr[i].c_str());
			yy_switch_to_buffer(testBuffer);
			REQUIRE(yylex() == tEnm[i]);
			yy_delete_buffer(testBuffer);
		}
	}

}
/*Similar to first test case, but for each key words I loop through
a set of garbage strings that I append to the end of the keywords
before inputing them to the lexer. This should simulate a fair portion
of cases where a keyword is close to some other token*/
/*
TEST_CASE("Test Case for keywords with suffix garbage","[tokens]") {

	YY_BUFFER_STATE testBuffer;

	for(auto i=0; i< static_cast<int>(tStr.size());++i){
		for(auto j=0; j < static_cast<int>(tGarbage.size());++j){
			SECTION("garbage test for enum #" + std::to_string(i) + " with suffix: " + tGarbage[j]){
				testBuffer = yy_scan_string((tStr[i]+tGarbage[j]).c_str());
				yy_switch_to_buffer(testBuffer);
				REQUIRE(yylex() == tEnm[i]);
				yy_delete_buffer(testBuffer);
			}
		}
	}

}
*/

/*Makes sure everything in the list tEndl is an endline character
*/
TEST_CASE("More endline tests","[tokens]") {

	const std::vector<std::string>  tEndl = {"\n", "\r", "\n\r", "\r\n"};

	YY_BUFFER_STATE testBuffer;

	for(auto i=0; i < static_cast<int>(tEndl.size());++i){
		SECTION("Plain test for newline character " + tEndl[i]){
			testBuffer = yy_scan_string(tEndl[i].c_str());
			yy_switch_to_buffer(testBuffer);
			REQUIRE(yylex() == ENDL);
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
                "\"[\"","\"\\\"","\"]\"","\"^\"","\"_\"","\"`\"","\"a\"","\"b\"","\"c\"",
                "\"d\"","\"e\"","\"f\"","\"g\"","\"h\"","\"i\"","\"j\"","\"k\"","\"l\"",
                "\"m\"","\"n\"","\"o\"","\"p\"","\"q\"","\"r\"","\"s\"","\"t\"","\"u\"",
                "\"v\"","\"w\"","\"x\"","\"y\"","\"z\"","\"{\"","\"|\"","\"}\"","\"~\"",
                "\"     \"","\"\\n\"","\"\\r\"","\"\\n\\r\"","\"\\r\\n\"",
                "\"\\a\"", "\"\\b\"", "\"\\f\"", "\"\\t\"", "\"\\v\"",
                "\"longer string of stuff\""};

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
			REQUIRE(yylex() == IDENTIFIER);
			yy_delete_buffer(testBuffer);
		}
	}

}
/*Makes sure everything in the list tErr returns the ERROR token
*/
TEST_CASE("make sure some strings throw errors","[tokens]") {

	const std::vector<std::string>  tErr = {"!", "_man", "_mayn", "_anythingelse"};

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
    REQUIRE(yylex() == LET);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == TYPE);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == EQUAL);
    REQUIRE(yylex() == ARRAY);
    REQUIRE(yylex() == OF);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == VAR);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == COLON);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == ASSIGNMENT);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == LBRACKET);
    REQUIRE(yylex() == INTLIT);
    REQUIRE(yylex() == RBRACKET);
    REQUIRE(yylex() == OF);
    REQUIRE(yylex() == INTLIT);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == IN);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == END);
    REQUIRE(yylex() == ENDL);
    fclose(yyin);
}

TEST_CASE("make sure real file test2.tig works","[tokens]") {
    yyin = fopen("tiger-programs/test2.tig", "r");
    REQUIRE(yylex() == LET);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == TYPE);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == EQUAL);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == TYPE);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == EQUAL);
    REQUIRE(yylex() == ARRAY);
    REQUIRE(yylex() == OF);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == VAR);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == COLON);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == ASSIGNMENT);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == LBRACKET);
    REQUIRE(yylex() == INTLIT);
    REQUIRE(yylex() == RBRACKET);
    REQUIRE(yylex() == OF);
    REQUIRE(yylex() == INTLIT);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == IN);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == END);
    REQUIRE(yylex() == ENDL);
    fclose(yyin);
}

TEST_CASE("make sure test3.tig works","[tokens]") {
    yyin = fopen("tiger-programs/test3.tig", "r");
    REQUIRE(yylex() == LET);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == TYPE);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == EQUAL);
    REQUIRE(yylex() == LBRACE);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == COLON);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == COMMA);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == COLON);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == RBRACE);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == VAR);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == COLON);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == ASSIGNMENT);
    REQUIRE(yylex() == LBRACE);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == EQUAL);
    REQUIRE(yylex() == STRINGLIT);
    REQUIRE(yylex() == COMMA);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == EQUAL);
    REQUIRE(yylex() == INTLIT);
    REQUIRE(yylex() == RBRACE);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == IN);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == DOT);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == ASSIGNMENT);
    REQUIRE(yylex() == STRINGLIT);
    REQUIRE(yylex() == SEMICOLON);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == IDENTIFIER);
    REQUIRE(yylex() == ENDL);
    REQUIRE(yylex() == END);
    REQUIRE(yylex() == ENDL);
    fclose(yyin);
}
