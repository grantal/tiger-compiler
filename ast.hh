/* Companion source code for "flex & bison", published by O'Reilly
 * Media, ISBN 978-0-596-15597-1
 * Copyright (c) 2009, Taughannock Networks. All rights reserved.
 * See the README file for license conditions and contact info.
 * $Header: /home/johnl/flnb/code/RCS/fb3-1.h,v 2.1 2009/11/08 02:53:18 johnl Exp $
 */
/*
 * Declarations for a calculator fb3-1
 */

#include <functional>
#include <string>
#include <cmath>


/* interface to the lexer */
void yyerror(char *s, ...);

namespace tiger {

///////////////////////////////////////////////////////////////////////////////
// Base AST node class, to define the hierarchy.
class ASTNode {
 public:
  using token_t = int;  // our token enum evaluates to int
  using char_t = char;
  using string_t = std::string; // for nodes defined by strings, ">=", "<>" etc
  using ASTptr = const ASTNode*; // Can't use smart ptr in union :(

  ASTNode() = default;
  virtual ~ASTNode() = default;
  virtual std::string toStr() const = 0; // For printing purposes
};

///////////////////////////////////////////////////////////////////////////////
// class for tokens like ID, String literals or interger literals that stores
// their token and their string value. So one may have token = ID and 
// value = "_main". They also can't have children
class TokenASTNode : public ASTNode {
 public:
  TokenASTNode(token_t token, string_t value)
   : ASTNode(), token_(token), value_(value)
  {}
  virtual ~TokenASTNode() = default;

  virtual string_t toStr() const
  {
    return std::to_string(token_) + ": " + value_;
  }
 private:
  const token_t token_;
  const string_t value_;
};

} // namespace
