#include "catch.hpp"
#include "token.hh"
#include "lex.yy.c"
#include <string>

/*
int yylex() {
    return 0;
}

void yy_scan_buffer(std::string base){
   base.cbegin(); 
}
*/

TEST_CASE("individual elements", "[tokens]") {
    
    /*
    COMMA,COLON,SEMICOLON,
    LPAREN,RPAREN,LBRACKET,RBRACKET,
    LBRACE,RBRACE,DOT,PLUS,MINUS,STAR,
    FSLASH,EQUAL,NOTEQUAL,LESS,GREATER,
    ELESS,EGREATER,AND,OR,
    ASSINGMENT,ENDL,STRINGLIT,INTLIT,
    IDENTIFIER,ERROR;
    */
    SECTION("comma"){
        yy_scan_buffer(",", 1);
        REQUIRE(yylex() == COMMA);
    }
    SECTION("colon"){
        yy_scan_buffer(":", 1);
        REQUIRE(yylex() == COLON);
    }
    SECTION("semicolon"){
        yy_scan_buffer(";", 1);
        REQUIRE(yylex() == SEMICOLON);
    }
    SECTION("lparen"){
        yy_scan_buffer("(", 1);
        REQUIRE(yylex() == LPAREN);
    }
}
