/* symantic_checks.cc
 * place for our symatic checks
 */
#include <iostream>
#include "semantic_checks.hh"
#include "tiger.tab.h"

namespace tiger {

// gonna need to beef this function up later
void semantic_error(ASTNode::ASTptr node, std::string error){
   std::cerr << "Error at " << node->getLine() << "," << node->getCol() << ": "; 
   std::cerr << error << std::endl; 
}

#define UNUSED(x) (void)(x)
// tells you what type an ast evaluates to
// give it 1+1, it will return int
// **maybe we should have an "invalid" type since we will want to get the type of both
// literal values and references, and if a reference is not defined, it will probably be
// easier to do so from the place where we call this function.
Scope::type_t typeOf(ASTNode::ASTptr node, std::shared_ptr<Scope> env){
    UNUSED(node);
    UNUSED(env);
    return "int";
}

//Should create type from recVals and be able to handle being passed node==nullptr.
Scope::rec_t recTypeOf(ASTNode::ASTptr node, std::shared_ptr<Scope> env){
    UNUSED(node);
    UNUSED(env);
    Scope::rec_t retRecT;
    return retRecT;
}

int semantic_checks(ASTNode::ASTptr node, std::shared_ptr<Scope> env) {
    if (const ParentASTNode* parNode = dynamic_cast<const ParentASTNode*>(node)) {
        // print full ast
        switch (parNode->getNodeType()) {
            case nodeType::PROGRAM:
                return semantic_checks(parNode->_getChild(0), env);
                // don't need break since we return
            // make new env that we'll add to with our decs and use in our exps
            case nodeType::ARRAY_DEC:{
                int checks = 0;
                Scope::type_t id = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(0)), env);
                if (!env->isVar(id)){
                    semantic_error(parNode, "Array type " + id + " does not exist.");
                    checks++;
                }
                Scope::type_t size = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(1)), env);
                if (size != "int"){
                    semantic_error(parNode, "Array size must be of type int. got type: " + size + ".");
                    checks++;
                }
                Scope::type_t elementType = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(2)), env);
                if(env->getArrayType(id) != elementType){
                    semantic_error(parNode, "Array type mismatch, default array element does not match array type.");
                    checks++;
                }
                return checks;
            }
            case nodeType::RECORD_DEC:{
                int checks = 0;
                Scope::type_t id = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(0)), env);
                if (!env->isVar(id)){
                    semantic_error(parNode, "Record type " + id + " does not exist.");
                    checks++;
                }

                Scope::rec_t recType = recTypeOf(parNode->_getChild(1), env);
                if(env->getRecTypes(id) != recType){
                    semantic_error(parNode, "Record type mismatch, declared record does not match record type.");
                    checks++;
                }
                return checks;
            }
            case nodeType::WHILE_DO:
            case nodeType::IF_THEN:{
                int checks = 0;
                Scope::type_t condType = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(0)), env);
                if(condType != "int"){
                    semantic_error(parNode, "Conditional must be integer, got: " + condType);
                    checks++;
                }
                // Create newEnv for each flow of control (true and false(else))
                std::shared_ptr<Scope> newEnvTrue = std::make_shared<Scope>(*env);
                checks += semantic_checks(parNode->_getChild(1),newEnvTrue);

                if(parNode->_getChild(2) != nullptr){
                    std::shared_ptr<Scope> newEnvFalse = std::make_shared<Scope>(*env);
                    checks += semantic_checks(parNode->_getChild(2),newEnvFalse);
                }

                return checks;

            }
            case nodeType::FOR_TO_DO:{
                int checks = 0;
                Scope::type_t condType = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(1)), env);
                //not sure what type the first expr has to be, I think it can be anything
                if(condType != "int"){
                    semantic_error(parNode, "Conditional must be integer, got: " + condType);
                    checks++;
                }
                checks += semantic_checks(parNode->_getChild(0),env);
                checks += semantic_checks(parNode->_getChild(2),env);
                // The Do statement
                std::shared_ptr<Scope> newEnv = std::make_shared<Scope>(*env);
                checks += semantic_checks(parNode->_getChild(3),newEnv);

                return checks;

            }
            case nodeType::LET_IN_END: {
                // copy env to newEnv
                std::shared_ptr<Scope> newEnv = std::make_shared<Scope>(*env);
                auto letChild = parNode->_getChild(0);
                auto inChild = parNode->_getChild(1);
                int check1 = 0;
                int check2 = 0;
                if(letChild != nullptr){
                    check1 = semantic_checks(parNode->_getChild(0), newEnv);
                }
                if(inChild != nullptr){
                    check2 = semantic_checks(parNode->_getChild(1), newEnv);
                }
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
            case nodeType::VAR_DEC:{
                ASTNode::string_t id = dynamic_cast<const TokenASTNode*>(parNode->_getChild(0))->getVal();  
                if (env->isVar(id)){
                    semantic_error(parNode, "variable " + id + "already exists");
                    return 1;
                }
                auto expType = typeOf(parNode->_getChild(1), env);
                // this is the case that we are given a type
                if (parNode->numChildren() == 3){
                    ASTNode::string_t typeId = dynamic_cast<const TokenASTNode*>(dynamic_cast<const ParentASTNode*>(parNode->_getChild(2))->_getChild(0))->getVal();  
                    if (env->isType(typeId)) {
                        // type of variable needs to be type of expression
                        if (typeId != expType){
                            semantic_error(parNode, "variable " + id + " of type " + typeId + " was set to an expression of type " + expType);
                            return 1;
                        }
                    } else {
                        semantic_error(parNode, "variable has invalid type: " + typeId);
                        return 1;
                    }
                }
                // can only create the variable if the expression is okay
                if(semantic_checks(parNode->_getChild(1), env) == 0){
                    env->emplaceVar(id, expType);
                    return 0;
                }
                return 1;
            }
            //uniary integer opperations
            case nodeType::NEGATE:{
                Scope::type_t uniType = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(0)), env);
                if (uniType != "int"){
                    semantic_error(parNode, "Expression be integer, got:" + uniType);
                    return 1;
                }
                return 0;
            }
            //Binary Interger Opperations
            case nodeType::ADD:
            case nodeType::SUB:
            case nodeType::MULT:
            case nodeType::DIV:{
                int report = 0;
                Scope::type_t leftType = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(0)), env);
                Scope::type_t rightType = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(1)), env);
                if (leftType != "int"){
                    semantic_error(parNode, "Left hand side must be integer, got:" + leftType);
                    report++;
                }
                if (rightType != "int"){
                    semantic_error(parNode, "Right hand side must be integer, got:" + leftType);
                    report++;
                }
                return report;
            }
            //Binary Interger/Stinrg Opperations
            case nodeType::GREATER:
            case nodeType::LESSER:
            case nodeType::EQ_GREATER:
            case nodeType::EQ_LESS:{
                int report = 0;
                Scope::type_t leftType = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(0)), env);
                Scope::type_t rightType = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(1)), env);
                if (leftType != "int" || leftType != "string"){
                    semantic_error(parNode, "Left hand side must be integer or string, got: " + leftType);
                    report++;
                }
                if (rightType != "int" || rightType != "string"){
                    semantic_error(parNode, "Right hand side must be integer or string, got: " + rightType);
                    report++;
                }
                if (rightType != leftType){
                    semantic_error(parNode, "The Left and Right hand side must match, got: " + leftType + " and " + rightType);
                    report++;
                }
                return report;
            }
            case nodeType::EQUAL:
            case nodeType::NOT_EQUAL:{
                int report = 0;
                Scope::type_t leftType = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(0)), env);
                Scope::type_t rightType = typeOf(dynamic_cast<const TokenASTNode*>(parNode->_getChild(1)), env);
                if (leftType == "invalid"){
                    semantic_error(parNode, "Left hand side is invalid");
                    report++;
                }
                if (rightType == "invalid"){
                    semantic_error(parNode, "Right hand side is invalid");
                    report++;
                }
                if (rightType != leftType){
                    semantic_error(parNode, "The Left and Right hand side must match, got: " + leftType + " and " + rightType);
                    report++;
                }
                return report;
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
