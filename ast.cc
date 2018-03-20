/* ast.cc
 * implementation of ast class
 */
#include <algorithm>
#include <typeinfo>
#include <iostream>
#include "ast.hh"


namespace tiger{

ASTNode::string_t TokenASTNode::toStr(int depth) const {
    string_t retStr = "";
    // insert two spaces for each level of depth
    for (int i = 0; i < depth; i++) {
        retStr += "  ";
    }
    return retStr + std::to_string(token_) + ": " + value_;
}

ParentASTNode::~ParentASTNode(){
    std::for_each(children_.begin(), children_.end(), [](ASTptr c){
        if (c != nullptr) {
            delete c;
        }
    }); 
}

ASTNode::string_t ParentASTNode::toStr(int depth) const {
    string_t retStr = "";
    // insert two spaces for each level of depth
    for (int i = 0; i < depth; i++) {
        retStr += "  ";
    }
    // insert this nodes description
    retStr += desc_ + " {\n";
    // insert children description
    std::for_each(children_.begin(), children_.end(), [&retStr, &depth](ASTptr c){
        if (c != nullptr) {
            retStr += c->toStr(depth + 1) + "\n";
        }
    }); 
    for (int i = 0; i < depth; i++) {
        retStr += "  ";
    }
    return retStr + "}";
}
} //namespace
