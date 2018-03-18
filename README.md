# tiger-compiler

This is a compiler for the [tiger language](https://www.cs.princeton.edu/~appel/modern/) in C and C++. We use flex and bison. Written by Alex Grant and Matt Atteberry.

## Lexer notes

### comments
Whenever we see a `/*` we enter a 'comment mode' state. No tokens are returned in comment mode. Whenever we see a `/*`, we increment a counter. Whenever we see a `*/`, we decrement it. When the counter hits zero, we exit comment mode to the INITIAL state
### strings
Anything between two quotation marks that does not contain illegal escape or a newline is returned by our lexer as a string literal. The legal escapes are `\a`, `\b`, `\f`, `\n`, `\r`, `\t`, `\v`, `\\`, `\"`, `\OCTNUM`, `\xHEXNUM` where OCTNUM and HEXNUM and valid octal and hexadecimal numbers respectively.
### errors
Anything that is not hit by one of our rules is counted as an error token
### end-of-file
When `yylex()` returns 0, we say we've reached the end of the file
### other edge cases
* unterminated strings return the ERROR token
* If there is an unterminated comment, we just treat it as if the file ends at the beginning of the comment

## Parser Notes

### Our AST classes

##### TokenASTNode 

We use two different kinds of `ASTNode` classes to build our Abstract Syntax Tree. First is `TokenASTNode` which does not have any children and stores the lexical token of a certain thing and it's literal string. This is used for string litrals, int literals, identifiers and nil. So one node may store a token of `INTLIT` and a string of `"32"`.

##### ParentASTNode

This node stores another enum called `nodeType` that says what this node does. Like is it a for loop, or is it a let/in clause. It also stores a description string which is used for printing. Then, it stores a vector of child ASTNodes. So the ParentASTNode for the expression `while 1 do "str"` would have a children vector that looks like: `{TokenASTNode(INTLIT, "1"), TokenASTNode(STRINGLIT, "\"str\"")}`.

### programNode

`programNode` is global shared pointer that we use to get the abstract syntax tree from `tiger.y` to `test_parser.cc`. The variable is declared in `tiger.y` and whenever we match a full program, we make a new `ParentASTNode` and set the `programNode` pointer to that new node.

### Array creation vs array indexing

One of the biggest conflicts we had was how to tell `typeId [ exp ] of exp` from `id [ exp ]` (where `typeId -> id`). If the parser had infinite lookahead, it would see the `of` and there would be no problem, but it can only see the `[` and it needs to decide what to do with the id. What we did was change the first rule to `id [ exp ] of exp` and have it create a `typeId` node in the action for that rule, rather than in the `typeId` rule. 
