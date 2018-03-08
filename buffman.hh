/**
Buffer manager
Pass it a string, it will switch to a buffer with that string
pass it a file, it will switch to buffer on that file
*/

// couldn't figure out how to get YY_BUFF_SIZE with extern so I put this block in
// YY_BUF_SIZE should never be defined in this file and if it is, then lex.yy.c isn't loaded
// and the 'extern "C"' block will fail.
// this just prevents the compiler from getting mad at me for YY_BUF_SIZE being undefined
#ifndef YY_BUF_SIZE
#define YY_BUF_SIZE 16384
#endif

typedef struct yy_buffer_state* YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char*);
extern YY_BUFFER_STATE yy_create_buffer(FILE*,int);
extern void yy_switch_to_buffer(YY_BUFFER_STATE);
extern void yy_delete_buffer(YY_BUFFER_STATE);

class Buffman {
private:
    YY_BUFFER_STATE buff_;
    FILE* f_;
public:
    Buffman(std::string s) :buff_{} {
        buff_ = yy_scan_string(s.c_str());
        yy_switch_to_buffer(buff_);
    }
    Buffman(FILE* f) :buff_{}, f_{f} {
        buff_ = yy_create_buffer(f_, YY_BUF_SIZE);
        yy_switch_to_buffer(buff_);
    }
    ~Buffman() {
        yy_delete_buffer(buff_);
        //fclose(f_);
    }
}; 

