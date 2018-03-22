#pragma once
#include <memory>
#include "symt.hh"
#include "ast.hh"

namespace tiger {

extern int semantic_checks(ASTNode::ASTptr node, std::shared_ptr<Scope> env = std::make_shared<Scope>());

}// namespace
