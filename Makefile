FX=flex   # flex compiler
BS=bison
C=gcc
CXX=clang++-5.0   # Can switch to g++ if desired
# CXX=g++-6
CXXFLAGS=-g -Og -std=c++17 -Wall -pedantic -Wextra -Werror
#CXXFLAGS=-O3 -std=c++17 -Wall -pedantic -Wextra -Werror
LDFLAGS=$(CXXFLAGS)
LIBS=-lfl

all: testlexer

testlexer: lex.yy.o test_lexer.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

lex.yy.o: lex.yy.c
	$(C) -c -o $@ $^

lex.yy.c: tiger.l
	$(FX) tiger.l

testparser: lex.yy.o tiger.tab.o test_parser.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

tiger.tab.o: tiger.tab.c lex.yy.c
	$(C) -c $^

tiger.tab.%: tiger.y
	$(BS) -d $^

%.o.cc: %.cc %.hh
	$(CXX) $(CFLAGS) -c -o $@ $<

    

test: all
	./testlexer

clean:
	rm -f *.o *.gch testlexer lex.yy.c tiger.tab.h tiger.tab.c
