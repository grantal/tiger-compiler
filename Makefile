
FX=flex   # flex compiler
C=gcc
CXX=clang++-5.0   # Can switch to g++ if desired
# CXX=g++-6
CXXFLAGS=-g -Og -std=c++17 -Wall -pedantic -Wextra -Werror
#CXXFLAGS=-O3 -std=c++17 -Wall -pedantic -Wextra -Werror
LDFLAGS=$(CXXFLAGS)
LIBS=-lfl

all: testFlex

testFlex: lex.yy.o testFlex.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

lex.yy.o: lex.yy.c
	$(C) -c -o $@ $^

lex.yy.c: tiger.l
	$(FX) tiger.l

%.o.cc: %.cc %.hh
	$(CXX) $(CFLAGS) -c -o $@ $<

test: all
	./testFlex

clean:
	rm -f *.o testFlex lex.yy.c
