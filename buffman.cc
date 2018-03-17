/**
buffman.cc
implementation of the Buffman class, the buffer manager
*/
#include "buffman.hh"

// couldn't figure out how to get YY_BUFF_SIZE with extern so I put this block in
// YY_BUF_SIZE should never be defined in this file and if it is, then lex.yy.c isn't loaded
// and the 'extern "C"' block will fail.
// this just prevents the compiler from getting mad at me for YY_BUF_SIZE being undefined
#ifndef YY_BUF_SIZE
#define YY_BUF_SIZE 16384
#endif

// flex/bison functions we need
extern YY_BUFFER_STATE yy_scan_string(const char*);
extern YY_BUFFER_STATE yy_create_buffer(FILE*,int);
extern void yy_switch_to_buffer(YY_BUFFER_STATE);
extern void yy_delete_buffer(YY_BUFFER_STATE);
extern int yylineno;

namespace buffman {

Buffman::Buffman(std::string s) :buff_(), f_(nullptr) {
    yylineno = 1;
    buff_ = yy_scan_string(s.c_str());
    yy_switch_to_buffer(buff_);
}
Buffman::Buffman(FILE* f) :buff_(), f_(f) {
    yylineno = 1;
    buff_ = yy_create_buffer(f_, YY_BUF_SIZE);
    yy_switch_to_buffer(buff_);
}
Buffman::~Buffman() {
    yy_delete_buffer(buff_);
    if(f_) {
        fclose(f_);
    }
}

} //namespace
