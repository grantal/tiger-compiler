FX=flex   # flex compiler
BS=bison
CXX=clang++-5.0   # Can switch to g++ if desired
CXXFLAGS=-g -Og -std=c++17 -Wall -pedantic -Wextra -Werror
OLDFLAGS=-Wno-write-strings -Wno-deprecated -std=c++17 -c #for compiling c code with c++ compiler
LDFLAGS=$(CXXFLAGS)
LIBS=-lfl

all: testlexer testparser

testlexer: lex.yy.o test_lexer.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

lex.yy.o: lex.yy.c
	$(CXX) $(OLDFLAGS) -o $@ $^

lex.yy.c: tiger.l
	$(FX) tiger.l

testparser: lex.yy.o tiger.tab.o test_parser.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

tiger.tab.o: tiger.tab.c lex.yy.c
	$(CXX) $(OLDFLAGS) $^

tiger.tab.%: tiger.y
	$(BS) -d $^

%.o.cc: %.cc %.hh
	$(CXX) $(CFLAGS) -c -o $@ $<

test: all
	./testlexer && ./testparser

clean:
	rm -f *.o *.gch testlexer lex.yy.c tiger.tab.h tiger.tab.c
