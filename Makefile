FX=flex   # flex compiler
BS=bison
CXX=clang++-5.0   # Can switch to g++ if desired
CXXFLAGS=-g -Og -std=c++17 -Wall -pedantic -Wextra -Werror
OLDFLAGS=-Wno-write-strings -Wno-deprecated -std=c++17 -c #for compiling c code with c++ compiler
LDFLAGS=$(CXXFLAGS)
LIBS=-lfl

all: testlexer testparser testsemantics

testlexer: lex.yy.o tiger.tab.o test_lexer.o ast.o buffman.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

lex.yy.o: lex.yy.c tiger.tab.h tiger.tab.c
	$(CXX) $(OLDFLAGS) lex.yy.c tiger.tab.c

lex.yy.c: tiger.l
	$(FX) tiger.l

testparser: lex.yy.o tiger.tab.o test_parser.cc ast.o buffman.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

tiger.tab.o: tiger.tab.c lex.yy.c
	$(CXX) $(OLDFLAGS) $^

tiger.tab.%: tiger.y
	$(BS) -d $^

testsemantics: lex.yy.o tiger.tab.o semantic_checks.o test_semantics.cc ast.o buffman.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

%.o.cc: %.cc %.hh
	$(CXX) $(CFLAGS) -c -o $@ $<

test: all
	./testlexer && ./testparser && ./testsemantics

clean:
	rm -f *.o *.gch testlexer lex.yy.c tiger.tab.h tiger.tab.c
