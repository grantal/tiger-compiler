/* ast.hh
 * based off of ast.hh by Eitan Frachtenberg
 * api for our Abstract Syntax Tree classes
 * There are 3 main subclass of ASTNode:
 * TokenASTNode, which takes represents a terminal token that has an associated Enum token
 * StringASTNode, for terminal tokens that do not have an associated Enum
 * ParentASTNOde, like StringASTNode but for nonterminals
 */
#pragma once
#include <string>
#include <vector>

/* interface to the lexer */
void yyerror(char *s, ...);

namespace tiger {

///////////////////////////////////////////////////////////////////////////////
// Base AST node class, to define the hierarchy.
class ASTNode {
 public:
  using token_t = int;  // our token enum evaluates to int
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

///////////////////////////////////////////////////////////////////////////////
// class for tokens like <, <=, [, ], <>, etc. just stores their literal value
class StringASTNode : public ASTNode {
 public:
  StringASTNode(string_t value)
   : ASTNode(), value_(value)
  {}
  virtual ~StringASTNode() = default;

  virtual string_t toStr() const
  {
    return value_;
  }
 private:
  const string_t value_;
};

///////////////////////////////////////////////////////////////////////////////
// Stores a vector of its children and a string describing what it does 
// So you may have a node with desc of "typeId" and a single child thats a token
// Node with token "ID"
class ParentASTNode : public ASTNode {
 public:
  ParentASTNode(string_t desc, std::vector<ASTptr> children)
   : ASTNode(), desc_(desc), children_(children)
  {}
  virtual ~ParentASTNode();
  virtual string_t toStr() const;    
  virtual string_t getDesc() const
  {
    return desc_;
  }
  ASTptr _getChild(int index) const
  {
    return children_[index];
  }
 private:
  const string_t desc_;
  const std::vector<ASTptr> children_;
};

} // namespace
