#ifndef __BINDINGS_HH
#define __BINDINGS_HH


#include <value.hh>
#include <state.hh>


#define wulf_binding(name) extern "C" value::Object* name(int argc, value::Object **argv, State* state, scope::Scope* scope)


#endif
