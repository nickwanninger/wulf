#ifndef __BINDINGS_HH
#define __BINDINGS_HH


#include <value.hh>
#include <state.hh>

#define wulf_binding_sig(name) value::obj name(int argc, value::obj *argv, State* state, scope::Scope *scope)
#define wulf_binding(name) \
	wulf_binding_sig(name) asm ("_" #name); \
	wulf_binding_sig(name)


#endif
