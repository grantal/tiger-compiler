#include "catch.hpp"
#include "token.hh"
#include "lex.yy.c"
#include <string>


TEST_CASE("individual elements", "[tokens]") {
    
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
    SECTION("rparen"){
        yy_scan_buffer(")", 1);
        REQUIRE(yylex() == RPAREN);
    }
    SECTION("lbracket"){
        yy_scan_buffer("[", 1);
        REQUIRE(yylex() == LBRACKET);
     }
    SECTION("rbracket"){
        yy_scan_buffer("]", 1);
        REQUIRE(yylex() == RBRACKET);
    }
    SECTION("lbrace"){
        yy_scan_buffer("{", 1);
        REQUIRE(yylex() == LBRACE);
     }
    SECTION("rbrace"){
        yy_scan_buffer("}", 1);
        REQUIRE(yylex() == RBRACE);
    }
    SECTION("dot"){
        yy_scan_buffer(".", 1);
        REQUIRE(yylex() == DOT);
    }
    SECTION("plus"){
        yy_scan_buffer("+", 1);
        REQUIRE(yylex() == PLUS);
    }
    SECTION("minus"){
        yy_scan_buffer("-", 1);
        REQUIRE(yylex() == MINUS);
    }
    SECTION("star"){
        yy_scan_buffer("*", 1);
        REQUIRE(yylex() == STAR);
    }
    SECTION("fslash"){
        yy_scan_buffer("/", 1);
        REQUIRE(yylex() == FSLASH);
    }
    SECTION("equal"){
        yy_scan_buffer("=", 1);
        REQUIRE(yylex() == EQUAL);
    }
    SECTION("notequal"){
        yy_scan_buffer("<>", 2);
        REQUIRE(yylex() == NOTEQUAL);
    }
    SECTION("less"){
        yy_scan_buffer("<", 1);
        REQUIRE(yylex() == LESS);
    }
    SECTION("greater"){
        yy_scan_buffer(">", 1);
        REQUIRE(yylex() = GREATER);
    }
    SECTION("eless"){
        yy_scan_buffer("=<", 2);
        REQUIRE(yylex() == ELESS);
    }
    SECTION("egreater"){
        yy_scan_buffer("=>", 2);
        REQUIRE(yylex() = EGREATER);
    }
    SECTION("and"){
        yy_scan_buffer("&", 1);
        REQUIRE(yylex() == AND);
    }
    SECTION("or"){
        yy_scan_buffer("|", 1);
        REQUIRE(yylex() = OR);
    }
    SECTION("assignment"){
        yy_scan_buffer(":=", 2);
        REQUIRE(yylex() == ASSIGNMENT);
    }
    SECTION("endl"){
        yy_scan_buffer("\n \r \r\n \n\r", 9);
        REQUIRE(yylex() = ENDL);
        REQUIRE(yylex() = ENDL);
        REQUIRE(yylex() = ENDL);
        REQUIRE(yylex() = ENDL);
    }
    SECTION("stringlit"){
        yy_scan_buffer("\"hi\"", 4);
        REQUIRE(yylex() == STRINGLIT);
    }
    SECTION("intlit"){
        yy_scan_buffer("0 1 2 3 4 5 6 7 8 9", 19);
        REQUIRE(yylex() == INTLIT);
        REQUIRE(yylex() == INTLIT);
        REQUIRE(yylex() == INTLIT);
        REQUIRE(yylex() == INTLIT);
        REQUIRE(yylex() == INTLIT);
        REQUIRE(yylex() == INTLIT);
        REQUIRE(yylex() == INTLIT);
        REQUIRE(yylex() == INTLIT);
        REQUIRE(yylex() == INTLIT);
    }
    SECTION("identitfier"){
        yy_scan_buffer("id", 2);
        REQUIRE(yylex() == IDENTIFIER);
    }
    SECTION("error"){
        yy_scan_buffer("!err", 4);
        REQUIRE(yylex() == ERROR);
    }
}
