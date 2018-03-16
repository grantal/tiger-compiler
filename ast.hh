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

enum class nodeType{
	PROGRAM,		//tiger.y 51-52
	ARRAY, 			//tiger.y 59
	EMPTY_REC, 		//tiger.y 60,
	NEW_REC, 		//tiger.y 60-61
	REC_VAL, 		//tiger.y 105 - 106
	NEW_ID, 		//tiger.y 63
	REF, 			//tiger.y (reference) 65-66, 116-119
	CALL_FUNC,		//tiger.y 68-69
	CALL_METHOD,	//tiger.y 71-72
	NEGATE, 		//tiger.y 74
	ADD, 			//tiger.y 75
	SUB, 			//tiger.y 76
	MULT, 			//tiger.y 77
	DIV, 			//tiger.y 78
	EQUAL, 			//tiger.y 79
	NOT_EQUAL, 		//tiger.y 80
	GREATER, 		//tiger.y 81
	LESSER, 		//tiger.y 82
	EQ_GREATER, 	//tiger.y 83
	EQ_LESS, 		//tiger.y 84
	AND, 			//tiger.y 85
	OR, 			//tiger.y 86
	SEQUENCE, 		//tiger.y 87, 124-125 (? unsure)
	EXP_LIST,		//tiger.y 110-111
	ASSIGNMENT_, 	//tiger.y 89-90
	IF_THEN, 		//tiger.y 92-93
	WHILE_DO, 		//tiger.y 94
	FOR_TO_DO, 		//tiger.y 95
	LET_IN_END, 	//tiger.y 97-100
	DECS, 			//tiger.y 130-131
	TYPE_DEC, 		//tiger.y 135
	CLASS_DEC, 		//tiger.y 137-138
	VAR_DEC, 		//tiger.y 140 (?), 154-155
	FUNC_DEC, 		//tiger.y 142-143
	PRIM_DEC, 		//tiger.y 145-146
	IMPORT_, 		//tiger.y 148
	CLASS_FIELDS, 	//tiger.y 159-160
	CLASS_FIELD, 	//tiger.y 164-168
	TYPE_ID, 		//tiger.y 171
	REC_TYPE, 		//tiger.y 173-174, 190-191
	ARRAY_TYPE, 	//tiger.y 176
	CLASS_TYPE, 	//tiger.y 178-181
        TY_FIELDS               //tiger.y 202-203
};

///////////////////////////////////////////////////////////////////////////////
//tiger.y Base AST node class, to define the hierarchy.
class ASTNode {
 public:
  using token_t = int;  //tiger.y our token enum evaluates to int
  using string_t = std::string; //tiger.y for nodes defined by strings, ">=", "<>" etc
  using ASTptr = const ASTNode*; //tiger.y Can't use smart ptr in union :(

  ASTNode() = default;
  virtual ~ASTNode() = default;
  virtual std::string toStr() const = 0; //tiger.y For printing purposes
};

///////////////////////////////////////////////////////////////////////////////
//tiger.y class for tokens like ID, String literals or interger literals that stores
//tiger.y their token and their string value. So one may have token = ID and 
//tiger.y value = "_main". They also can't have children
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
//tiger.y class for tokens like <, <=, [, ], <>, etc. just stores their literal value
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
//tiger.y Stores a vector of its children and a string describing what it does 
//tiger.y So you may have a node with desc of "typeId" and a single child thats a token
//tiger.y Node with token "ID"
class ParentASTNode : public ASTNode {
 public:
  ParentASTNode(string_t desc, nodeType nType, std::vector<ASTptr> children)
   : ASTNode(), desc_(desc), nodeType_(nType), children_(children)
  {}
  virtual ~ParentASTNode();
  virtual string_t toStr() const;    
  virtual string_t getDesc() const
  {
    return desc_;
  }
  // returns child for testing
  ASTptr _getChild(int index) const
  {
    return children_[index];
  }

  virtual nodeType getNodeType() const
  {
  	return nodeType_;
  }

 private:
  const string_t desc_;
  const nodeType nodeType_;
  const std::vector<ASTptr> children_;
};


} //namespace
