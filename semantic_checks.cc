/* symantic_checks.cc
 * place for our symatic checks
 */
#include <iostream>
#include "semantic_checks.hh"
#include "tiger.tab.h"

namespace tiger {

/*
 * sends given string to cerr with line and column numbers of error
 */
void semantic_error(ASTNode::ASTptr node, std::string error){
   std::cerr << "Error at " << node->getLine() << "," << node->getCol() << ": "; 
   std::cerr << error << std::endl; 
}

#define TYPELESS "" //type for a statement with no type like x := 1

bool typeCheck(Scope::type_t LHS, Scope::type_t RHS,std::shared_ptr<Scope> env){
	if(env->isPrimitiveType(RHS)){
		return env->getPrimitiveType(LHS) == RHS;
	}
	return LHS == RHS;
}

Scope::type_t semantic_checks_helper(ASTNode::ASTptr node, std::shared_ptr<Scope> env,int &checks) {
    if (const ParentASTNode* parNode = dynamic_cast<const ParentASTNode*>(node)) {
        switch (parNode->getNodeType()) {
            case nodeType::PROGRAM:
                return semantic_checks_helper(parNode->_getChild(0), env, checks);
                // don't need break since we return
            // make new env that we'll add to with our decs and use in our exps
            case nodeType::ARRAY_DEC:{ // <id> [size] of <element type>
                Scope::type_t id = semantic_checks_helper(parNode->_getChild(0),env,checks);
                if (!env->isType(id)){
                    semantic_error(parNode, "Array type " + id + " does not exist.");
                    checks++;
                }
                Scope::type_t size = semantic_checks_helper(parNode->_getChild(1),env,checks);
                if (size != "int"){
                    semantic_error(parNode, "Array size must be of type int. got type: " + size + ".");
                    checks++;
                }
                Scope::type_t element = semantic_checks_helper(parNode->_getChild(2),env,checks);
                if(env->getArrayType(id) != element){
                    semantic_error(parNode, "Array type mismatch, default array element does not match array type.");
                    checks++;
                }
                return id;
            }
            case nodeType::RECORD_DEC:{// <id> {}
                Scope::type_t id = semantic_checks_helper(parNode->_getChild(0),env,checks);
                if (!env->isType(id)){
                    semantic_error(parNode, "Record type " + id + " does not exist.");
                    checks++;
                }

                //retrieve recVals[valID := value] into rec_t
                Scope::rec_t recType;
                ASTNode::ASTptr currentNode = parNode->_getChild(1);
                Scope::type_t valueID;
                Scope::type_t valueType;

                while(currentNode != nullptr){
                    valueID = semantic_checks_helper(parNode->_getChild(0),env,checks);
                    valueType = semantic_checks_helper(parNode->_getChild(1),env,checks);
                    if(!env->isType(valueID)){
                        semantic_error(parNode, "Record Value type " + valueID + " does not exist.");
                        checks++;
                    }
                    if(!typeCheck(valueID,valueType,env)){//might need something more sophiscated for this comparison later
                        semantic_error(parNode, "Record Value type mismatch, declared record value does not match record value type.");
                        checks++;
                    }
                    recType.push_back(valueID);
                }

                if(env->getRecTypes(id) != recType){
                    semantic_error(parNode, "Record type mismatch, declared record does not match record type.");
                    checks++;
                }
                return id;
            }
            case nodeType::WHILE_DO: {
                semantic_checks_helper(parNode->_getChild(0),env,checks);
                auto bodyType = semantic_checks_helper(parNode->_getChild(1),env,checks);
                // the body of the while needs to be typeless
                if (bodyType != TYPELESS){
                    semantic_error(parNode, "Body of while loop must produce no value. Instead it produces value of type " + bodyType + ".");
                    checks++;
                    return TYPELESS; 
                }
                return TYPELESS;
            }
            case nodeType::IF_THEN:{
                Scope::type_t condType = semantic_checks_helper(parNode->_getChild(0),env,checks);
                if(condType != "int"){
                    semantic_error(parNode, "Conditional must be integer, got: " + condType);
                    checks++;
                }
                // Create newEnv for each flow of control (true and false(else))
                std::shared_ptr<Scope> newEnvTrue = std::make_shared<Scope>(*env);
                semantic_checks_helper(parNode->_getChild(1),newEnvTrue,checks);

                if(parNode->_getChild(2) != nullptr){
                    std::shared_ptr<Scope> newEnvFalse = std::make_shared<Scope>(*env);
                    semantic_checks_helper(parNode->_getChild(2),newEnvFalse,checks);
                }

                return "";

            }
            case nodeType::FOR_TO_DO:{
                auto lowType = semantic_checks_helper(parNode->_getChild(1),env,checks);
                auto hiType  = semantic_checks_helper(parNode->_getChild(2),env,checks);
                // low and high of the iterator must be ints
                if(lowType != "int"){
                    semantic_error(parNode, "iterator low must be integer, got: " + lowType);
                    checks++;
                }
                if(hiType != "int"){
                    semantic_error(parNode, "iterator high must be integer, got: " + hiType);
                    checks++;
                }
                semantic_checks_helper(parNode->_getChild(0), env,checks);
                // add the iterator variable to the env of the body
                // the variable "id" stores the name of the iterator
                ASTNode::string_t id = dynamic_cast<const TokenASTNode*>(parNode->_getChild(0))->getVal(); 
                std::shared_ptr<Scope> newEnv = std::make_shared<Scope>(*env);
                newEnv->inLoop = true;
                newEnv->emplaceVar(id, "int");
                newEnv->iterName = id;
                semantic_checks_helper(parNode->_getChild(3),newEnv,checks);
                return TYPELESS;
            }
            case nodeType::LET_IN_END: {
                // copy env to newEnv
                std::shared_ptr<Scope> newEnv = std::make_shared<Scope>(*env);
                auto letChild = parNode->_getChild(0);
                auto inChild = parNode->_getChild(1);
                if(letChild != nullptr){
                    semantic_checks_helper(parNode->_getChild(0), newEnv,checks);
                }
                if(inChild != nullptr){
                    semantic_checks_helper(parNode->_getChild(1), newEnv,checks);
                }
                return ""; // will error if either check errors 
            } // newEnv gets deleted here when it goes out of scope
            case nodeType::DECS:
                if (parNode->numChildren() == 1){
                    semantic_checks_helper(parNode->_getChild(0), env,checks);
                } else if (parNode->numChildren() == 2){
                    semantic_checks_helper(parNode->_getChild(0), env,checks);
                    semantic_checks_helper(parNode->_getChild(1), env,checks);
                } 
                // if there are not one or two children, something has gone terribly wrong
                return "";
            case nodeType::TYPE_DEC:{ 
                Scope::type_t id = semantic_checks_helper(parNode->_getChild(0), env,checks);
                if (env->isType(id)) {
                    semantic_error(parNode, "attempting to redefine type: " + id);
                    checks++;
                }

                Scope::type_t baseType  = semantic_checks_helper(parNode->_getChild(1), env,checks);
                env->emplaceType(id,baseType);
                return "";
            }
            case nodeType::VAR_DEC:{
                semantic_checks_helper(parNode->_getChild(0), env,checks);
                // make sure varible with this id has not been declared
                ASTNode::string_t id = dynamic_cast<const TokenASTNode*>(parNode->_getChild(0))->getVal(); 
                if (env->isVar(id)){
                    semantic_error(parNode, "variable " + id + "already exists");
                }
                Scope::type_t expType = semantic_checks_helper(parNode->_getChild(1), env,checks);
                // this is the case that we are given a type
                if (parNode->numChildren() == 3){
                    Scope::type_t typeId = semantic_checks_helper(parNode->_getChild(2), env,checks); 
                    if (env->isType(typeId)) {
                        // type of variable needs to be type of expression
                        if (typeCheck(typeId, expType, env)){ //*might neet more sophisticated comparion
                            semantic_error(parNode, "variable " + id + " of type " + typeId + " was set to an expression of type " + expType);
                            checks++;
                        }
                    } else {
                        semantic_error(parNode, "variable has invalid type: " + typeId);
                        checks++;
                    }
                }
                env->emplaceVar(id, expType);
                return "";
            }
            //uniary integer opperations
            case nodeType::NEGATE:{
                Scope::type_t uniType = semantic_checks_helper(parNode->_getChild(0), env,checks);
                if (uniType != "int"){
                    semantic_error(parNode, "Expression be integer, got:" + uniType);
                    checks++;
                }
                return "int";//Not sure if we should return what we actually get, or what it should be. Feels redundant to
                            //to return what we go if its incorrect since every node up the chain will report an error as well.
            }
            //Binary Interger Opperations
            case nodeType::ADD:
            case nodeType::SUB:
            case nodeType::MULT:
            case nodeType::DIV:{
                Scope::type_t leftType = semantic_checks_helper(parNode->_getChild(0), env,checks);
                Scope::type_t rightType = semantic_checks_helper(parNode->_getChild(1), env,checks);
                if (leftType != "int"){
                    semantic_error(parNode, "Left hand side must be integer, got:" + leftType);
                    checks++;
                }
                if (rightType != "int"){
                    semantic_error(parNode, "Right hand side must be integer, got:" + leftType);
                    checks++;
                }
                return "int";
            }
            //Binary Interger/Stinrg Opperations
            case nodeType::GREATER:
            case nodeType::LESSER:
            case nodeType::EQ_GREATER:
            case nodeType::EQ_LESS:{
                Scope::type_t leftType = semantic_checks_helper(parNode->_getChild(0), env,checks);
                Scope::type_t rightType = semantic_checks_helper(parNode->_getChild(1), env,checks);
                if (leftType != "int" || leftType != "string"){
                    semantic_error(parNode, "Left hand side must be integer or string, got: " + leftType);
                    checks++;
                }
                if (rightType != "int" || rightType != "string"){
                    semantic_error(parNode, "Right hand side must be integer or string, got: " + rightType);
                    checks++;
                }
                if (rightType != leftType){
                    semantic_error(parNode, "The Left and Right hand side must match, got: " + leftType + " and " + rightType);
                    checks++;
                }
                return "int"; //similarly to above, trying to return what should be the correct type
            }
            case nodeType::EQUAL:
            case nodeType::NOT_EQUAL:{
                Scope::type_t leftType = semantic_checks_helper(parNode->_getChild(0), env,checks);
                Scope::type_t rightType = semantic_checks_helper(parNode->_getChild(1), env,checks);
                if (leftType == "invalid"){
                    semantic_error(parNode, "Left hand side is invalid");
                    checks++;
                }
                if (rightType == "invalid"){
                    semantic_error(parNode, "Right hand side is invalid");
                    checks++;
                }
                if (rightType != leftType){
                    semantic_error(parNode, "The Left and Right hand side must match, got: " + leftType + " and " + rightType);
                    checks++;
                }
                return "int"; //bool 1 or 0
            }
            case nodeType::ASSIGNMENT_: {
                semantic_checks_helper(parNode->_getChild(0), env,checks);
                semantic_checks_helper(parNode->_getChild(1), env,checks);
                // check if we are trying to assign to the iterator of the for loop we're in 
                if(auto idNode = dynamic_cast<const TokenASTNode*>(parNode->_getChild(0))){ 
                    if(idNode->getVal() == env->iterName){
                        semantic_error(parNode, "Attempting to assign to iterator variable " + env->iterName + ".");
                        checks++;
                    }
                }
                return TYPELESS;
            }
            default:
                return "";
        }
    }
    else if (const TokenASTNode* tokNode = dynamic_cast<const TokenASTNode*>(node)) {
        //Need to implement acutal integer values for INTLIT STRINGLIT values
        tiger::ASTNode::token_t tokType = tokNode->getToken();
        switch(tokType) {
        case INTLIT:
            return "int";
        case STRINGLIT:
            return "string";
        case BREAK: 
            // if we see a break, we must be inside a loop
            if(!env->inLoop) {
                semantic_error(tokNode, "\"break\" outside of loop");
                checks++;
            }
        default:
            return "";
        }
    } 
    // not TokenNode or ParentNode, so something went wrong
    else {
        semantic_error(node, "Node in AST that is neither TokenASTNode nor ParentASTNode");
        checks++;
        return "";
    }
};

int semantic_checks(ASTNode::ASTptr node, std::shared_ptr<Scope> env){
    int checks = 0;
    semantic_checks_helper(node, env, checks);
    return checks;
}

} //namespace
