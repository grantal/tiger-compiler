/* ast.cc
 * implementation of ast class
 */
#include "ast.hh"


namespace tiger{

ParentASTNode::~ParentASTNode(){
    std::for_each(children_.begin(), children_.end(), [](ASTptr c){ delete c;}); 
}

ASTNode::string_t ParentASTNode::toStr() const {
    std::string retStr = desc_ + " {\n";
    std::for_each(children_.begin(), children_.end(), [&retStr](ASTptr c){
        retStr += "\t" + c->toStr() + "\n";
    }); 
    return retStr + "}";
}
} //namespace
