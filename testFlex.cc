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
	//extern FILE* yyin;
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
                                  "comma","colon","semicolon",
                                  "lparen","rparen","lbracket","rbracket",
                                  "lbrace","rbrace","dot","plus","minus","star",
                                  "fslash","equal","notequal","less","greater",
                                  "eless","egreater","and","or",
                                  "assignment","endl","stringlit","intlit",
                                  "identifier","error"};

	const std::vector<std::string> tGarbage = {"array","if","then",
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
