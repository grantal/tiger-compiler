#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "token.hh"
#include <stdio.h>
#include <iostream>

extern "C"{
	extern int yylex(void);
	typedef struct yy_buffer_state* YY_BUFFER_STATE;
	extern YY_BUFFER_STATE yy_scan_string(const char*);
	extern void yy_switch_to_buffer(YY_BUFFER_STATE);
	extern void yy_delete_buffer(YY_BUFFER_STATE);
	//extern FILE* yyin;
}

TEST_CASE("Test Case for IF") {

  YY_BUFFER_STATE testBuffer = yy_scan_string("IF");
  yy_switch_to_buffer(testBuffer);
  while(int tok = yylex()){
  	std::cout << tok << std::endl;
  	REQUIRE(tok == IF);
  }
  yy_delete_buffer(testBuffer);

}
