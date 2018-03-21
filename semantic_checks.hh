#pragma once
#include "symt.hh"
#include "ast.hh"

namespace tiger {

extern int semantic_checks(ASTNode::ASTptr node, Scope* env = new Scope());

}// namespace
