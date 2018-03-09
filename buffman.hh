/**
Buffer manager
Pass it a string, it will switch to a buffer with that string
pass it a file, it will switch to buffer on that file
*/
#pragma once
#include <cstdio>
#include <string>


typedef struct yy_buffer_state* YY_BUFFER_STATE;

class Buffman {
private:
    YY_BUFFER_STATE buff_;
    FILE* f_;
public:
    Buffman(std::string s);
    Buffman(FILE* f);
    ~Buffman();
}; 

