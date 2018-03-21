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

namespace tiger {

class Scope {
  public:
    using type_t = std::string; 
    using TypeT = std::set<type_t>;
    // symbol table is a map from identifiers (strings) to types (strings)
    // I may need to change sym_val_t later if I need to store more than just the type
    using sym_id_t = std::string;
    using sym_val_t = std::string;
    using SymT = std::map<sym_id_t, sym_val_t>;

    // constructors and destructor
    Scope() = default;   
    Scope(const Scope &other) = default;   
    virtual ~Scope() = default;
        
    // insertType and emplaceType act basically the same
    // is seemed uneven to have an insert and emplace for var and func and not have both for
    // type
    virtual void insertType(type_t newT) {
       types_.insert(newT);
    }
    virtual void emplaceType(type_t newT) {
       types_.emplace(newT);
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

  private:
    TypeT types_; 
    SymT vars_;
    SymT funcs_;
};

// default types in tiger
Scope::TypeT defaultTypes = {"int", "string", "array", "record"};

} // namespace
