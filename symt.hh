/* symt.hh
 * api for our symbol tables
 * We have 3 classes. First is TypeT which acts as a set of all of the types of the current
 * scope as string.
 * Then we have SymT  which contains a map from an identifier to it's type
 * also a class 'Scope' that stores a TypeT and 2 SymTs, one for functions and one for variables
 */
#pragma once
#include <string>
#include <set>
#include <map>
#include <vector>

namespace tiger {

class Scope {
  public:
    using type_t = std::string;
    using PrimTypeT = std::set<type_t>;
    using TypeT = std::map<type_t,type_t>;
    using rec_t = std::vector<type_t>;
    using array_t = type_t; //should only ever have two entries
    using RecTypeT = std::map<type_t,rec_t>;
    using ArrayTypeT = std::map<type_t,array_t>;
    // symbol table is a map from identifiers (strings) to types (strings)
    // I may need to change sym_val_t later if I need to store more than just the type
    using sym_id_t = std::string;
    using sym_val_t = std::string;
    using SymT = std::map<sym_id_t, sym_val_t>;
  private:
    // default types in tiger
    const PrimTypeT defaultTypes = {"int", "string", "array", "record",""};

  public:
    // constructors and destructor
    Scope(): inLoop(false), iterName(""), primTypes_(defaultTypes), types_(), vars_(), funcs_() {};   
    Scope(const Scope &other) = default;   
    virtual ~Scope() = default;
        
    // insertType and emplaceType act basically the same
    // is seemed uneven to have an insert and emplace for var and func and not have both for
    // type
    virtual void insertType(std::pair<type_t,type_t> newV) {
        if(!isType(newV.first)){
            types_.insert(newV);
        }
    }
    virtual void emplaceType(type_t newT, type_t baseT) {
        if(!isType(newT)){
            types_.emplace(newT, baseT);
        }
    }
    // I think the emplace functions will be far more useful
    virtual void insertVar(std::pair<sym_id_t,sym_val_t> newV) {
       vars_.insert(newV);
    }
    virtual void emplaceVar(sym_id_t newV, sym_val_t newT) {
       vars_.emplace(newV, newT);
    }
    virtual void insertFunc(std::pair<sym_id_t,sym_val_t> newF) {
       funcs_.insert(newF);
    }
    virtual void emplaceFunc(sym_id_t newF, sym_val_t newT) {
       funcs_.emplace(newF, newT);
    }
    // see if a given string is a valid type
    virtual bool isType(type_t t) const {
        return types_.find(t) != types_.end();
    }
    // see if a given string is a primitive type
    virtual bool isPrimitiveType(type_t t) const {
        return defaultTypes.find(t) != primTypes_.end();
    }
    // get type of variable or function
    virtual sym_val_t getVarType(sym_id_t v) {
        return vars_.at(v);
    } 
    virtual sym_val_t getFuncType(sym_id_t f) {
        return funcs_.at(f);
    } 
    // get type list of a record type
    virtual rec_t getRecTypes(type_t rT) {
        return recTypeT_[rT];
    }
    // get type of elements of array type
    virtual type_t getArrayType(type_t aT) {
        return arrayTypeT_[aT];
    }
    // check if variable or function exist
    virtual bool isVar(sym_id_t v) {
        return vars_.find(v) != vars_.end();
    } 
    virtual bool isFunc(sym_id_t f) {
        return funcs_.find(f) != funcs_.end();
    }
    virtual type_t getUserType(type_t t){
        if(types_.find(t) == types_.end()){
            return "";
        };
        return types_.at(t);
    }
    virtual type_t getPrimitiveType(type_t t){
        type_t currentType = t;
        while(!isPrimitiveType(currentType)){
            currentType = getUserType(currentType);
            if(currentType == ""){
                return ""; //return "" becuase failed to get to a primitive 
            }
        }
        return currentType;
    }
    // these variables track if we're in a loop and what the iterator variable
    // name is
    bool inLoop;
    sym_id_t iterName;

  private:
    RecTypeT recTypeT_;
    ArrayTypeT arrayTypeT_;
    PrimTypeT primTypes_;
    TypeT types_; 
    SymT vars_;
    SymT funcs_;
};


} // namespace
