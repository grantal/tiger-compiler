# tiger-compiler

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
