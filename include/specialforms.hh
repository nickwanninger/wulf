#ifndef __SPECIALFORMS_HH
#define __SPECIALFORMS_HH

#include <wulf.hh>
#include <vector>

NSCLASS(value, Object);
NSCLASS(scope, Scope);
class State;

typedef std::vector<value::Object> valuelist;
typedef value::Object (*specialformfn)(State*, scope::Scope*, valuelist);

#define SPECIALFORM(name) value::Object name(State*, scope::Scope*, valuelist)

namespace specialform {
//SPECIALFORM(add);
//SPECIALFORM(sub);
//SPECIALFORM(mul);
//SPECIALFORM(div);
//SPECIALFORM(pow);
//SPECIALFORM(print);
//SPECIALFORM(quote);
//SPECIALFORM(eval);
//SPECIALFORM(load);
//SPECIALFORM(lambda);
//SPECIALFORM(set);
//SPECIALFORM(setq);
//SPECIALFORM(gc_collect);
//SPECIALFORM(if_stmt);
//SPECIALFORM(defun);
//SPECIALFORM(repl);
//SPECIALFORM(equals);
//SPECIALFORM(greater);
//SPECIALFORM(less);
//SPECIALFORM(nand);
//
//SPECIALFORM(first);
//SPECIALFORM(rest);
}

#endif
