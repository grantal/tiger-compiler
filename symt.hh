/* symt.hh
 * api for our symbol tables
 * We have 3 classes. First is TypeT which acts as a set of all of the types of the current
 * scope as string.
 * Then we have SymT  which contains a map from an identifier to it's type
 * also a class 'Scope' that stores a TypeT and 2 SymTs, one for functions and one for variables
 */
#pragma once
#include <string>
#include <iterator>
#include <set>
#include <map>
#include <vector>

namespace tiger {

class Scope {
  public:
    using type_t = std::string;
    using PrimTypeT = std::set<type_t>;
    using TypeT = std::map<type_t,type_t>;

    // symbol table is a map from identifiers (strings) to types (strings)
    // I may need to change sym_val_t later if I need to store more than just the type
    using sym_id_t = std::string;
    using sym_val_t = std::string;
    using SymT = std::map<sym_id_t, sym_val_t>;

    //===============|||ARRAY TYPES|||======================
    using array_t = type_t; //type of array elements
    using ArrayTypeT = std::map<type_t,array_t>;

    using sym_array_t = std::vector<array_t>;
    using ArraySymT = std::map<sym_val_t, sym_array_t>;
    //=================================================

    //===============|||RECORD TYPES|||======================
    //type of <id = typeOfValue> pairs stored in record table 
    using recVal_t = std::pair<sym_id_t,type_t>;
    //special record type pointed to from type table
    using rec_t = std::vector<recVal_t>;
    using RecTypeT = std::map<type_t,rec_t>;

    using recSym_val_t = std::pair<sym_id_t,sym_val_t>;
    using sym_rec_t = std::vector<recSym_val_t>;
    using RecSymT = std::map<sym_val_t, sym_rec_t>;
    //=================================================
  private:
    // default types in tiger
    const PrimTypeT defaultTypes = {"int", "string", "array", "record",""};

  public:
    // constructors and destructor
    Scope(): inLoop(false), iterName(""), primTypes_(defaultTypes), types_(), vars_(), varTypes_(), funcs_() {};   
    Scope(const Scope &other) = default;   
    virtual ~Scope() = default;
        
    virtual void insertType(std::pair<type_t,type_t> newV) {
        if(!isType(newV.first)){
            types_.insert(newV);
        }
    }
    virtual void emplaceType(type_t newT, type_t baseT) {
        if(!isType(newT)){
            types_.emplace(newT, baseT);
        }
        // uncomment the next line if you want to print types_
        //for (auto it=types_.begin(); it!=types_.end(); ++it) std::cout << it->first << " => " << it->second << '\n';
    }
    // I think the emplace functions will be far more useful
    virtual void insertVar(std::pair<sym_id_t,sym_val_t> newV) {
       vars_.insert(newV);
    }
    virtual void emplaceVar(sym_id_t newV, sym_val_t newT, type_t varType) {
       vars_.emplace(newV, newT);
       varTypes_.emplace(newV, varType);
    }
    virtual void insertFunc(std::pair<sym_id_t,sym_val_t> newF) {
       funcs_.insert(newF);
    }
    virtual void emplaceFunc(sym_id_t newF, sym_val_t newT) {
       funcs_.emplace(newF, newT);
    }
    // see if a given string is a valid type
    virtual bool isType(type_t t) const {
        return (types_.find(t) != types_.end()) || (primTypes_.find(t) != primTypes_.end());
    }
    // see if a given string is a primitive type
    virtual bool isPrimitiveType(type_t t) const {
        return defaultTypes.find(t) != primTypes_.end();
    }
    // get variable or function value
    virtual sym_val_t getVar(sym_id_t v) {
        return vars_.at(v);
    } 
    // get variable Type
    virtual type_t getVarType(sym_id_t v) {
        return varTypes_.at(v);
    } 

    virtual sym_val_t getFunc(sym_id_t f) {
        return funcs_.at(f);
    } 
    // get type list of a record type
    virtual rec_t getRecTypes(type_t rT) {
        return recTypeT_[types_[rT]];
    }
    virtual sym_val_t getRecSymFromRec(sym_val_t recField ,sym_val_t recRef) {
        for(auto field : recs_.at(recRef)){
            if(field.first == recField){
                return field.second; //refernce to another record
            }
        }
        return ""; //return TYPELESS if fails to find record
    }
    // create an internal reference name between vars_ map and recs_ map
    virtual sym_val_t getRecSymRef() {
        return "!rec_" + std::to_string(recs_.size());
    }
    // create an internal reference name between types_ map and recTypeT_ map
    virtual sym_val_t getRecTypeRef() {
        return "!recTypeT_" + std::to_string(recTypeT_.size());
    }
    virtual void emplaceRec(sym_val_t newRecRef,sym_rec_t newRec,type_t recTypes){
        recs_.emplace(newRecRef, newRec);
        recTypes_.emplace(newRecRef, recTypes);
    }
    virtual void emplaceRecType(type_t newRecTypeRef,rec_t newRecType){
        recTypeT_.emplace(newRecTypeRef, newRecType);
    }
    // get variable Type
    virtual type_t getRecType(sym_val_t r) {
        return recTypes_.at(r);
    } 
    // get type of elements of array type
    virtual type_t getArrayType(type_t aT) {
        return arrayTypeT_[aT];
    }
    // check if variable or function exist
    virtual bool isVar(sym_id_t v) {
        return vars_.find(v) != vars_.end();
    }
    virtual bool isRec(sym_id_t r) {
        return recs_.find(r) != recs_.end();
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
    RecTypeT recTypeT_; //store links to record Type vectors
    RecSymT recs_;  //store links to sym records
    SymT recTypes_; //parralell to rec_ ; links to sym record type ids
    ArrayTypeT arrayTypeT_;
    PrimTypeT primTypes_;
    TypeT types_;
    ArraySymT arrays_;
    SymT vars_;
    SymT varTypes_;
    SymT funcs_;
};


} // namespace
