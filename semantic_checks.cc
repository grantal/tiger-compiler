/* symantic_checks.cc
 * place for our symatic checks
 */
#include <iostream>
#include "semantic_checks.hh"
#include "tiger.tab.h"

namespace tiger {

// gonna need to beef this function up later
void semantic_error(ASTNode::ASTptr node, std::string error){
   std::cerr << error << std::endl; 
   node->toStr(); //compiler got mad at me for unused parameter. We'll use this to get line numbers later
}

int semantic_checks(ASTNode::ASTptr node, std::shared_ptr<Scope> env) {
    if (const ParentASTNode* parNode = dynamic_cast<const ParentASTNode*>(node)) {
        // print full ast
        switch (parNode->getNodeType()) {
            case nodeType::PROGRAM:
                std::cout << parNode->toStr() << std::endl;
                return semantic_checks(parNode->_getChild(0), env);
                // don't need break since we return
            // make new env that we'll add to with our decs and use in our exps
            case nodeType::LET_IN_END: {
                // copy env to newEnv
                std::shared_ptr<Scope> newEnv = std::make_shared<Scope>(*env);
                int check1 = semantic_checks(parNode->_getChild(0), newEnv);
                int check2 = semantic_checks(parNode->_getChild(1), newEnv);
                return check1 + check2; // will error if either check errors 
            } // newEnv gets deleted here when it goes out of scope
            case nodeType::DECS:
                if (parNode->numChildren() == 1){
                    return semantic_checks(parNode->_getChild(0), env);
                } else if (parNode->numChildren() == 2){
                    int check1 = semantic_checks(parNode->_getChild(0), env);
                    int check2 = semantic_checks(parNode->_getChild(1), env);
                    return check1 + check2; // will error if either check errors 
                } 
                // if there are not one or two children, something has gone terribly wrong
                return 1;
            case nodeType::TYPE_DEC:{ 
                ASTNode::string_t id = dynamic_cast<const TokenASTNode*>(parNode->_getChild(0))->getVal();  
                if (env->isType(id)) {
                    semantic_error(parNode, "attempting to redefine type: " + id);
                    return 1;
                }        
                env->insertType(id);
                return semantic_checks(parNode->_getChild(1), env);
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
