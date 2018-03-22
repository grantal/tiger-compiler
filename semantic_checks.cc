/* symantic_checks.cc
 * place for our symatic checks
 */
#include <iostream>
#include "semantic_checks.hh"
#include "tiger.tab.h"

namespace tiger {

int semantic_checks(ASTNode::ASTptr node, std::shared_ptr<Scope> env) {
    if (const ParentASTNode* parNode = dynamic_cast<const ParentASTNode*>(node)) {
        // print full ast
        switch (parNode->getNodeType()) {
            case nodeType::PROGRAM:
                std::cout << parNode->toStr() << std::endl;
                return semantic_checks(parNode->_getChild(0), env);
                break;
            // make new env that we'll add to with our decs and use in our exps
            case nodeType::LET_IN_END: {
                std::shared_ptr<Scope> newEnv = std::make_shared<Scope>(*env); //this should copy env?
                int check1 = semantic_checks(parNode->_getChild(0), newEnv);
                int check2 = semantic_checks(parNode->_getChild(1), newEnv);
                // make sure newEnv actually is a copy
                newEnv->insertType("myCoolType");
                std::cout << env->isType("myCoolType") << std::endl;
                std::cout << newEnv->isType("myCoolType") << std::endl;
                return check1 + check2;
                break;
            } // newEnv gets deleted here when it goes out of scope
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
