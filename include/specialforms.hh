#ifndef __SPECIALFORMS_HH
#define __SPECIALFORMS_HH

#include <wulf.hh>
#include <vector>

NSCLASS(value, Value)
NSCLASS(scope, Scope)
class State;

#define SPECIALFORM(name) value::Value* name(State*, scope::Scope*, valuelist)
typedef std::vector<value::Value*> valuelist;
typedef value::Value* (*specialformfn)(State*, scope::Scope*, valuelist);

namespace specialform {
	SPECIALFORM(add);
	SPECIALFORM(sub);
	SPECIALFORM(mul);
	SPECIALFORM(div);
	SPECIALFORM(pow);
	SPECIALFORM(print);
	SPECIALFORM(quote);
	SPECIALFORM(eval);
	SPECIALFORM(load);
	SPECIALFORM(lambda);
	SPECIALFORM(set);
	SPECIALFORM(setq);
	SPECIALFORM(gc_collect);
	SPECIALFORM(if_stmt);
	SPECIALFORM(defun);
	SPECIALFORM(repl);
	SPECIALFORM(equals);
	SPECIALFORM(greater);
	SPECIALFORM(nand);
}

#endif
