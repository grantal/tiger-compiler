# tiger-compiler

This is a compiler for the [tiger language](https://www.cs.princeton.edu/~appel/modern/) in C and C++. We use flex and bison. Written by Alex Grant and Matt Atteberry.

## Symbol Tables
Our class for handling symbol tables is in `symt.hh`. We have a single class, `Scope` that stores:
 * Primitive types as a set
 * User defined types as a map from the name of the type to any type they were definied by
 * A map from array types to the type of their elements
 * A map from each defined variable, function, array and record to their type (4 maps)
 * A map from a record type to its key type
 * A map from a record type to its value type
 We have a bunch of getters and setters for all of these maps and sets.
 
## Handling Scopes
We have a `semantic_checks_helper` function which is recursive and a `Scope` variable gets passed to it. When we enter a new scope, we copy the `Scope` variable and pass the copy to `semantic_checks_helper` and recurse on the children of our `ASTNode`.
 
## Semantic error
We store the line and column number of the beginning of an `ASTNode` in the `ASTNode` object. We then have a function `semantic_error` that takes the node and reports the line and column of the beginning of the error along with a string describing the error
 
## List of Semantic Checks
### What works
* Making sure mutually recursive function definitions are uninterrupted
```
let

function do_nothing1(a: int, b: string):int=
		(do_nothing2(a+1);0)

var d:=0 /* interruption here */

function do_nothing2(d: int):string =
		(do_nothing1(d, "str");" ")

in
	do_nothing1(0, "str2")
end
```
* Type checking of `if then` and `if then else` statements
```
if 1 then 2          /* error: then should have no value */
if "3" then ()       /* error: condition should be of type int */
if 1 then 3 else "4" /* error: then and else need to have same type */
```
* Type checking of loop bodies and conditions
```
while 1 do 2               /* error: body of loop can return no value */
while "3" do ()            /* error: condition of while must be int */
for i := "blah" to 4 do () /* error: index variable must be int */
for i := 0 to 1 do 2       /* error: body of loop can return no value */
```
* Making sure `break` only gets called inside a loop
* Making sure for loop iterator does not get assigned to
```
for i := 0 to 1 do
  i := 2              /* error: assigning to i which is an iterator */
```
* Type checking for binary and unary operators
```
"3" + "4"  /* error: can't add strings */
"3" <> "4" /* this is okay */
"3" = 4    /* error: can't compare things of different types */
-"3"       /* error: can't negate strings */
```
* Mutually recursive type checking
```
/* error: mutually recursive types thet do not pass through record or array */
let 

type a=c
type b=a
type c=d
type d=a

in
 ""
end
```
* checking that variables/functions/types are declared
```
let 
  var j := 0
  var l:mySuperCoolType := 3 /* error: undelcared type mySuperCoolType */
in
  (f();                      /* error: undelcared function  f */
   i := 1)                   /* error: undeclared variable i */
end
```
* type checking for variable/function declaration
```
let 
  function f() : int = "3" /* error: function of type int returns string */
  function g() = 3         /* error: procedure returns value of type int */
  var i:int := "blah"       /* error: variable i of type int set to exp of type string */
in
  ()
end
```
* type checking for variable assignment
```
let
  var i := 1
in
  i := "1" /* error: var i of type int set to exp of type string */
end
```
### What doesnt work
* arrays
* records inside records inside record


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

### Implementing Tiger.y

The tiger grammar was implemented initially by closly following the syntax listed in the [Tiger Compiler Reference Manual](https://www.lrde.epita.fr/~tiger/tiger.html#Lexical-Specifications). Additional rules were added to support recursive syntax, like creating a record with either an arbitrary numebr of entries or no entries. For the empty record case, there is a rule to match `typeId '{' '}'`, and a seperate rule to match record entries. `%left` Precedence is defined for arithmetic and comparison operators to resolve shift/reduce conflicts, and `%nonassoc` used to resolve the 'hanging-else' conflict.

### Our AST classes

##### TokenASTNode 

We use two different kinds of `ASTNode` classes to build our Abstract Syntax Tree. First is `TokenASTNode` which does not have any children and stores the lexical token of a certain thing and it's literal string. This is used for string litrals, int literals, identifiers and nil. So one node may store a token of `INTLIT` and a string of `"32"`.

##### ParentASTNode

This node stores another enum called `nodeType` that says what this node does. Like is it a for loop, or is it a let/in clause. It also stores a description string which is used for printing. Then, it stores a vector of child ASTNodes. So the ParentASTNode for the expression `while 1 do "str"` would have a children vector that looks like: `{TokenASTNode(INTLIT, "1"), TokenASTNode(STRINGLIT, "\"str\"")}`.

### programNode

`programNode` is global shared pointer that we use to get the abstract syntax tree from `tiger.y` to `test_parser.cc`. The variable is declared in `tiger.y` and whenever we match a full program, we make a new `ParentASTNode` and set the `programNode` pointer to that new node.

### Array creation vs array indexing

One of the biggest conflicts we had was how to tell `typeId [ exp ] of exp` from `id [ exp ]` (where `typeId -> id`). If the parser had infinite lookahead, it would see the `of` and there would be no problem, but it can only see the `[` and it needs to decide what to do with the id. What we did was change the first rule to `id [ exp ] of exp` and have it create a `typeId` node in the action for that rule, rather than in the `typeId` rule. 
