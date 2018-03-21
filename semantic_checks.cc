/* symantic_checks.cc
 * place for our symatic checks
 */
#include <iostream>
#include "semantic_checks.hh"
#include "tiger.tab.h"

namespace tiger {

int semantic_checks(ASTNode::ASTptr node, Scope* env) {
    if (const ParentASTNode* parNode = dynamic_cast<const ParentASTNode*>(node)) {
        // print full ast
        switch (parNode->getNodeType()) {
            case nodeType::PROGRAM:
                std::cout << parNode->toStr() << std::endl;
                return semantic_checks(parNode->_getChild(0), env);
                break;
            // make new env that we'll add to with our decs and use in our exps
            case nodeType::LET_IN_END: {
                Scope* newEnv = new Scope(*env);
                semantic_checks(parNode->_getChild(0), newEnv);
                return semantic_checks(parNode->_getChild(1), newEnv);
                break;
            }
            default:
                return 0;
        }
    }
    else if (const TokenASTNode* tokNode = dynamic_cast<const TokenASTNode*>(node)) {
        return 0;
    } 
    // not TokenNode or ParentNode, so something went wrong
    else {
        return 1;
    }
};

} //namespace
