/* symt.hh
 * api for our symbol tables
 * We have 3 classes. First is TypeT which acts as a set of all of the types of the current
 * scope as string.
 * next is Sym which is a container with the string of its id, an enum of it's kind
 * (variable/function/method) and a string of its type
 * Then we have SymT  which stores the symbols for the current scope
 * also a class 'Scope' that stores a TypeT and a SymT and lets you operate on both
 */
#include <string>
#include <set>

namespace tiger {
    using TypeT = std::set<std::string>;
}
