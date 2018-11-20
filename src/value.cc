/*
 * A compiler for the wulf language
 * Copyright (C) 2018  Nick Wanninger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <wulf.hh>
#include <value.hh>
#include <vm.hh>
#include <locale>
#include <iostream>
#include <math.h>
#include <syscall.hh>
#include <gc/gc_cpp.h>

using namespace value;


Object::Object() {
}

Object::Object(Type t) {
	this->type = t;
}
Object::Object(double val) {
	type = value::number;
	number = val;
	Object();
}

Object::Object(char* str) {
	string = str;
	type = value::string;
}
Object::Object(const char* str) {
	string = ccharcopy(str);
	type = value::string;
}

Object::~Object() {
	// delete this;
	// std::cout << "Delete Object " << to_string() << "\n";
}

void* Object::operator new(size_t size) {
	return GC_MALLOC(size);
}

void Object::operator delete(void* ptr) {
	return GC_FREE(ptr);
}



void Object::write_stream(std::ostream & buf, bool human) {

	switch (type) {
		case unknown:
		case value::nil:
			buf << "nil";
			break;
		case value::list: {

				if (car == NULL) {
					buf << "()";
					return;
				}

				if (cdr == NULL) cdr = new value::Object(value::nil);
				if (is_pair()) {
					buf << "(" << car->to_string() << " . " << cdr->to_string() << ")";
					break;
				}
				buf << "(";
				Object* curr = this;
				while (curr->car != NULL) {
					buf << curr->car->to_string(false);
					if (curr->cdr != NULL) {
						if (curr->cdr->is_pair()) {
							buf << " " << curr->cdr->car->to_string()
								<< " . " << curr->cdr->cdr->to_string();
							break;
						}
						if (curr->cdr->car != NULL)
							buf << " ";
						curr = curr->cdr;
					}
				}
				buf << ")";
			}; break;

		case value::keyword:
		case value::ident:
			buf << string;
			break;

		case value::string:
			if (human) {
				buf << unescape(string);
			} else {
				buf << '"';
				buf << string;
				buf << '"';
			}
			break;
		case value::number: {
				auto str = std::to_string(number);
				long len = str.length();
				for (int i = len-1; i > 0; i--) {
					if (str[i] == '0') {
						str.pop_back();
					}	else if (str[i] == '.') {
						str.pop_back();
						break;
					} else {
						break;
					}
				}
				buf << str;
			}; break;

			// procedure stringifier
		case value::procedure:
			if (code->type == vm::bc_binding) {
				buf << "<procedure " << &code->binding << ">";
			} else if (code->type == vm::bc_normal) {
				buf << code->lambda->to_string();
			}
			break;
		case value::custom: {
				buf << "<";
				if (type_name == NULL) buf << "custom-type";
				else {
					buf << type_name;
				}
				buf << " ";
				buf << payload << ">";
			}; break;
	}
}



std::string Object::to_string(bool human) {
	std::ostringstream buf;

	write_stream(buf, human);
	std::string str = buf.str();

	return str;
}



#define BIN_CALL(name, op) \
	if (!strcmp(name, callname)) { if (arg_count == 2) {bc->push(vm::Instruction(op)); return; } else { throw "attempt to call binary math operator requires two arguments"; } }


#define UNARY_CALL(name, op) \
	if (!strcmp(name, callname)) { if (arg_count == 1) {bc->push(vm::Instruction(op)); return; } else { throw "unary operator requires one argument"; } }

#define ifcall(method) if (!strcmp(car->string, #method))


void Object::compile(vm::Machine* machine, scope::Scope* sc, vm::Bytecode* bc) {
	switch (type) {
		case value::unknown:
			bc->push(vm::Instruction(OP_NOP));
			return;

		case value::nil:
			bc->push(vm::Instruction(OP_PUSH_NIL));
			return;

		case value::list: {

				if (length() == 0) throw "invalid syntax ()";
				// car check if it's a special call or not. (quote, lambda, etc..)
				if (car->type == value::ident) {

					ifcall(set!) {
						if (length() != 3) throw "call to set! requires 2 argumenst";
						auto self = *this;
						value::Object name = *self[1];
						value::Object val = *self[2];
						if (name.type != value::ident) throw "name argument for set! must be an identifier literal";
						val.compile(machine, sc, bc);
						vm::Instruction inst(OP_STORE_LOCAL);
						inst.string = name.string;
						bc->push(inst);
						return;
					}

					ifcall(def) {
						if (cdr->car == NULL || cdr->cdr->car == NULL) throw "call to def requires at least two arguments";
						auto *name = cdr->car;

						// if the name is a list, I have to convert the syntactical sugar to
						// a more usable syntax with lambdas
						if (name->type == value::list) {
							// the following code is quite spooky. plz be careful.
							auto lambda = *newlist();
							if (name->car->type != value::ident)
								throw "call to def with function declaration syntax requires a function name as the car argument to the list";
							lambda.type = value::list;
							lambda.car = new value::Object("fn");
							lambda.car->type = value::ident;
							lambda.cdr = new value::Object(value::list);
							lambda.cdr->car = name->cdr;
							lambda.cdr->cdr = new value::Object();
							lambda.cdr->cdr->car = cdr->cdr->car;
							lambda.cdr->cdr->cdr = new value::Object();
							cdr->cdr->car = new value::Object(lambda);
							cdr->car = name->car;
							name = name->car;
						}

						if (name->type == value::ident) {
							auto *val = cdr->cdr->car;
							val->compile(machine, sc, bc);
							auto store = vm::Instruction(OP_STORE_LOCAL);
							store.string = name->string;
							bc->push(store);
							return;
						}

						throw "unexpected type in define name";
						return;
					}

					ifcall(syscall) {
						vm::Instruction inst(OP_SYSCALL);
						if (length() != 3) throw "syscall requires two arguments, a number and any value (can be nil if the syscall would ignore it)";

						inst.whole = length()-1;

						for (int i = 1; i < length(); i++) {
							(*this)[i]->compile(machine, sc, bc);
						}

						bc->push(inst);
						return;
					}


					// check for quote
					ifcall(quote) {
						if (cdr->car == NULL) throw "call to quote must have one argument";
						vm::Instruction inst(OP_PUSH_RAW);
						inst.opcode = OP_PUSH_RAW;
						inst.object = cdr->car;
						bc->push(inst);
						return;
					}

					ifcall(quasiquote) {
						cdr->car->compile_quasiquote(machine, sc, bc);
						return;
					}

					ifcall(unquote) {
						throw "unquote cannot be used outside of a quasi-quote (backtick)";
					}
					ifcall(unquote-splicing) {
						throw "unquote-splicing cannot be used outside of a quasi-quote (backtick)";
					}

					ifcall(defmacro) {
						value::Object self = *this;
						int len = length();

						if (len != 3)
							throw "macro definition requires 2 arguments (args and body)";

						auto* arg = operator[](1);
						auto* body = operator[](2);

						if (arg->type != value::list)
							throw "macro definition requires a list as the second argument. ex: (name ...)";

						if (arg->length() == 0)
							throw "macro definition must have a name";

						auto* name = arg->operator[](0);
						if (name->type != value::ident)
							throw "macro definition requires an identifier as a name";

						macro::Expansion *mac = new macro::Expansion();
						// get the name from the name argument
						mac->name = name->to_string();
						// because the cdr in the arg list could be NULL, (no args)
						// we have to conditionally parse the arguments
						arg = arg->cdr;
						if (arg != NULL) {
							mac->args = *value::parse_fn_args(*arg);
						} else {
							mac->args = std::vector<Argument>();
						}


						mac->body = body;
						machine->macros[name->to_string()] = mac;

						return;
					}

					ifcall(fn) {

						if (cdr->car == NULL) throw "'fn' call requires more arguments";
						if (length() == 2) throw "'fn' call missing body";
						auto *arglist = cdr->car;

						auto proc = new value::Object();
						proc->type = value::procedure;
						proc->code = new vm::Bytecode();

						// std::cout << to_string() << "\n";

						proc->args = value::parse_fn_args(*arglist);

						auto expr = operator[](2);
						expr->compile(machine, sc, proc->code);
						proc->code->push(OP_RETURN);
						proc->code->lambda = new value::Object(*this);

						vm::Instruction inst(OP_PUSH_FN);
						inst.object = proc;
						bc->push(inst);
						return;
					}

					ifcall(exit) {
						if (cdr->car == NULL) {
							vm::Instruction in(OP_PUSH_NUM);
							in.number = 0;
							bc->push(in);
						} else {
							cdr->car->compile(machine, sc, bc);
						}
						bc->push(OP_EXIT);
						return;
					}

					ifcall(do) {
						auto len = length();
						for (int i = 1; i < len; i++) {
							(*this)[i]->compile(machine, sc, bc);
							if (i < len-1) bc->push(OP_SKIP);
						}
						return;
					}


					ifcall(intern) {
						if (cdr->car == NULL) throw "call to intern requires an argument";
						cdr->car->compile(machine, sc, bc);
						bc->push(OP_INTERN);
						return;
					}

					ifcall(if) {
						// cache the len cause finding the length of a linked list
						// takes time.
						int len = length();
						auto self = *this;
						if (len < 2) throw "call to if requires a predicate, then, and optionally a false";
						if (len < 3) throw "call to if requires at least a truthy value";
						if (len < 4) append(new value::Object()); // if there isn't an else value, make it a nil

						self[1]->compile(machine, sc, bc);
						// push a branch onto the bytecode stack and keep a reference to it.
						// we'll wanna update this branch instruction's offset to the location
						// of the false code.
						// the car thing we do is push a nop to the jump index
						// we will update it's index later on.
						vm::Instruction branch(OP_JUMP_FALSE);
						long long bri = bc->instructions.size();
						bc->push(OP_NOP);
						self[2]->compile(machine, sc, bc);

						// we need to add unconditional branches to the joining block
						long long theni = bc->instructions.size();
						bc->push(OP_JUMP);


						branch.whole = bc->instructions.size();
						bc->instructions[bri] = branch;
						self[3]->compile(machine, sc, bc);
						bc->push(OP_NOP);
						bc->instructions[theni].whole = bc->instructions.size() - 1;;

						return;
					}
				}

				std::string callstr = car->to_string();
				const char* callname = callstr.c_str();

				// check if there is a macro for this function call
				if (machine->macros.count(callstr)) {
					// construct an argument list for the expansion
					std::vector<value::Object*> args;
					int len = length();
					for (int i = 1; i < len; i++) {
						args.push_back(operator[](i));
					}
					auto *macro = machine->macros[callstr];
					if (macro == NULL)
						throw "macro expansion exists but wasn't found. (NULL after hashtable lookup). ask a dev";
					// expand and compile the macro
					auto expansion = macro->expand(machine, args, sc);
					expansion->compile(machine, sc, bc);
					return;
				}

				int arg_count = 0;
				for (auto *arg = cdr; arg->type == value::list && arg->car != NULL; arg = arg->cdr) {
					arg->car->compile(machine, sc, bc);
					arg_count++;
				}

				BIN_CALL("and", OP_AND);
				BIN_CALL("or", OP_OR);
				UNARY_CALL("not", OP_NOT);

				// here's a little (not perfect) optimization.
				// it looks in the current scope to see if it can find a function definition
				// and if it does, it won't bother looking up the function at runtime
				try {

					value::Object *calling = sc->root->find(car->to_string());
					if (calling->type == value::procedure) {
						vm::Instruction in = OP_PUSH_RAW;
						in.object = calling;
						bc->push(in);
					}
				} catch (std::string msg) {
					// wasn't found, ignore it and push the car argumen to the stack
					car->compile(machine, sc, bc);
				}
				auto call = vm::Instruction(OP_CALL);
				call.whole = arg_count;
				bc->push(call);
				return;
			} break;

		case value::ident: {
				auto inst = vm::Instruction(OP_PUSH_LOOKUP);
				inst.string = string;
				if (!strcmp(string, "nil")) inst.opcode = OP_PUSH_NIL;
				bc->push(inst);
			}; break;

		default: {
				 auto inst = vm::Instruction(OP_PUSH_RAW);
				 inst.object = this;
				 bc->push(inst);
			 }; break;
	}
}



bool Object::is_call(const char* name) {
	if (type != value::list) return false;
	if (car == NULL) return false;
	if (car->type != value::ident) return false;
	if (strcmp(car->string, name)) return false;
	return true;
}

bool Object::is_pair() {
	if (cdr == NULL) cdr = new value::Object();
	if (type != value::list) return false;
	if (cdr->type == value::nil || cdr->type == value::list) return false;
	return true;
}

void Object::compile_quasiquote(vm::Machine* m, scope::Scope* sc, vm::Bytecode* bc) {

	// quasi-quote handles lists specially cause it has to check if it is a call to
	// unquote or unquote-splicing
	if (type == value::list && car != NULL) {

		if (is_call("unquote")) {
			value::Object *u = m->eval(cdr->car, sc);
			bc->push(OP_PUSH_RAW).object = u;
			return;
		}

		// the list wasnt a special call, we should
		// walk through the args and cons them together.
		int len = length();
		int consc = len;
		for (int i = 0; i < len; i++) {
			auto *elem = operator[](i);
			if (elem->is_call("unquote-splicing")) {
				value::Object *u = m->eval(elem->operator[](1), sc);
				if (u->type == value::list) {
					int ulen = u->length();
					consc += ulen-1;
					for (int j = 0; j < ulen; j++) {
						bc->push(OP_PUSH_RAW).object = u->operator[](j);
					}
				} else {
					bc->push(OP_PUSH_RAW).object = u;
				}
			} else {
				// if this isn't *really* a special call,
				// we'll just push it's raw form
				elem->compile_quasiquote(m, sc, bc);
			}
		}
		bc->push(OP_PUSH_NIL);
		for (int i = 0; i < consc; i++) bc->push(OP_CONS);
		return;
	}

	vm::Instruction & inst = bc->push(OP_PUSH_RAW);
	inst.object = this;
}

bool Object::is_true() {
	return type != value::nil;
}


size_t Object::length() {
	size_t len = 0;

	Object* curr = this;
	while (curr->cdr != NULL) {
		if (curr->car != NULL) len++;
		curr = curr->cdr;
	}
	return len;
}

Object* Object::operator[](int index) {
	size_t len = 0;
	Object* curr = this;
	while (curr->cdr != NULL) {
		if (len == index) return curr->car;
		if (curr->car != NULL) len++;
		curr = curr->cdr;
	}
	return NULL;
}


void Object::append(value::Object *obj) {
	if (type != value::list) throw "unable to push_back to non-list";
	auto curr = this;

	while (curr->cdr != NULL) {
		curr = curr->cdr;
	}

	curr->cdr = new value::Object(value::list);
	curr->cdr->type = value::list;
	curr->car = obj;
}


Object* value::newlist(std::vector<Object> items) {
	auto *v = new Object();
	v->type = value::list;
	for (Object item : items) {
		v->append(new value::Object(item));
	}
	return v;
}

Object* value::newident(const char* i) {
	auto *v = new Object();
	v->type = value::ident;
	v->string = strdup(i);
	return v;
}
Object* value::newstring(const char* i) {
	auto *v = new Object();
	v->type = value::string;
	v->string = strdup(i);
	return v;
}
Object* value::newnumber(double n) {
	auto *v = new Object();
	v->type = value::number;
	v->number = n;
	return v;
}

std::vector<Argument>* value::parse_fn_args(value::Object list) {
	auto* args = new std::vector<Argument>();

	if (list.type == value::list) {
		int len = list.length();
		for (int i = 0; i < len; i++) {
			value::Object arg = *list[i];
			if (arg.type == value::keyword) {
				if (i > len - 1)
					throw "keyword argument is missing it's ident argument";
				if (!strcmp(arg.string, ":rest")) {
					arg = *list[++i];
					if (arg.type != value::ident) throw ":rest argument modifier requires a following ident as a name";
					Argument a;
					a.type = value::rest;
					a.name = strdup(arg.string);
					args->push_back(a);
					if (i < len - 1) {
						throw ":rest argument must be the cdr argument";
					}
				} else
					throw "unknown keyword used in argument list";

			} else if (arg.type == value::ident) {
				Argument a;
				a.name = strdup(arg.string);
				args->push_back(a);
			} else {
				std::cout << list.to_string() << "\n";
				throw "Syntax Error: argument list in function must be idents or keywords";
			}
		}
	} else if (list.type == value::ident) {
		Argument a;
		a.name = list.string;
		a.type = value::plain;
		args->push_back(a);
	} else
			throw "invalid function argument syntax";
	return args;
}


void value::argument_scope_expand(std::vector<Argument> args, std::vector<value::Object*> objs, scope::Scope* sc) {

	int i, passedc;
	value::Argument argname;

	passedc = objs.size();


	for (i = 0; i < passedc; i++) {
		if (i >= args.size()) throw "too many arguments passed";
		argname = args[i];
		if (argname.type == value::plain) {
			sc->set(std::string(argname.name), objs[i]);
		} else if (argname.type == value::rest) {
			value::Object *lst = new value::Object(value::list);
			while (i < passedc)
				lst->append(objs[i++]);
			sc->set(std::string(argname.name), lst);
		} else throw "invalid number of arguments passed";

	}
}





value::Object *value::copy(value::Object *o) {
	return new value::Object(*o);
}
