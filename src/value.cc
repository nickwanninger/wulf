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

#include <value.hh>
#include <vm.hh>
#include <locale>

using namespace value;


Object::Object() {
}

Object::Object(Type t) {
	type = t;
}
Object::Object(double val) {
	type = value::number;
	number = val;
	Object();
}


Object::Object(char* str) {
	string = str;
}
Object::Object(const char* str) {
	string = strdup(str);
}

Object::~Object() {}


std::string Object::to_string() {
	std::ostringstream buf;

	switch (type) {
		case unknown:
		case value::nil:
			buf << "nil";
			break;
		case value::list: {
				if (first == NULL) return "()";
				buf << "(";
				Object* curr = this;
				while (curr->first != NULL) {
					buf << curr->first->to_string();
					if (curr->last != NULL) {
						buf << " ";
						curr = curr->last;
					}
				}
				std::string str = buf.str();
				str.pop_back();
				str += ")";
				return str;
			}; break;

		case value::keyword:
		case value::ident:
		case value::string:
			buf << string;
			break;

		case value::number:
			buf << number;
			break;

			// procedure stringifier
		case value::procedure:
			buf << "(lambda (";
			for (int i = 0; i < argc; i++) {
				buf << argv[i];
				if (i != argc-1) buf << " ";
			}
			buf << ") ...)";
			break;
	}


	return buf.str();
}


#define BIN_CALL(name, op) \
	if (!strcmp(name, callname)) { if (arg_count == 2) {bc->push(vm::Instruction(op)); return; } else { throw "attempt to call binary math operator with non-two arguments"; } }


#define UNARY_CALL(name, op) \
	if (!strcmp(name, callname)) { if (arg_count == 1) {bc->push(vm::Instruction(op)); return; } else { throw "unary operator requires one argument"; } }

#define ifcall(method) if (!strcmp(first->string, #method))
void Object::compile(vm::Machine* machine, vm::Bytecode* bc) {
	// std::cout << "compile " <<  to_string() << "\n";
	switch (type) {

		case value::unknown:
			bc->push(vm::Instruction(OP_NOP));
			return;

		case value::nil:
			bc->push(vm::Instruction(OP_PUSH_NIL));
			return;

		case value::list: {
				// first check if it's a special call or not. (quote, lambda, etc..)
				if (first->type == value::ident) {

					ifcall(def) {
						if (last->first == NULL || last->last->first == NULL) throw "call to define requires at least two arguments";
						auto *name = last->first;

						// if the name is a list, I have to convert the syntactical sugar to
						// a more usable syntax with lambdas
						if (name->type == value::list) {
							// the following code is quite spooky. plz be careful.
							auto lambda = value::Object();
							if (name->first->type != value::ident)
								throw "call to def with function declaration syntax requires a function name as the first argument to the list";
							lambda.type = value::list;
							lambda.first = new value::Object("lambda");
							lambda.first->type = value::ident;
							lambda.last = new value::Object(value::list);
							lambda.last->first = name->last;
							lambda.last->last = new value::Object();
							lambda.last->last->first = last->last->first;
							lambda.last->last->last = new value::Object();
							last->last->first = new value::Object(lambda);
							last->first = name->first;
							name = name->first;
						}

						if (name->type == value::ident) {
							auto *val = last->last->first;
							val->compile(machine, bc);
							auto store = vm::Instruction(OP_STORE_GLOBAL);
							store.string = name->string;
							bc->push(store);
							return;
						}

						throw "unexpected type in define name";
						return;
					}


					// check for quote
					ifcall(quote) {
						if (last->first == NULL) throw "call to quote must have one argument";
						last->first->compile_quote(machine, bc);
						return;
					}

					ifcall(print) {
						if (last->first == NULL) throw "call to print must have one argument";
						last->first->compile(machine, bc);
						bc->push(OP_PRINT);
						return;
					}

					ifcall(lambda) {
						if (last->first == NULL) throw "lambda call requires more arguments";
						if (length() == 2) throw "lambda call missing body";
						auto *arglist = last->first;

						auto proc = new value::Object();
						proc->type = value::procedure;
						proc->code = new vm::Bytecode();

						if (arglist->type == value::list) {
							proc->argc = arglist->length();
							proc->argv = new char*[proc->argc];
							for (int i = 0; i < proc->argc; i++) {
								value::Object *arg = (*arglist)[i];
								char* name = strdup(arg->to_string().c_str());
								proc->argv[i] = name;
							}
						} else if (arglist->type == value::ident) {
							proc->argc = 1;
							proc->argv = new char*[1];
							proc->argv[0] = arglist->string;
						} else {
							throw "syntax error in lambda construction. arguments must be a single ident or a list of idents";
							return;
						}

						auto expr = (*this)[2];


						expr->compile(machine, proc->code);
						proc->code->push(OP_RETURN);

						vm::Instruction inst(OP_PUSH_RAW);
						inst.object = proc;
						bc->push(inst);
						return;
					}

					ifcall(exit) {
						if (last->first == NULL) {
							vm::Instruction in(OP_PUSH_NUM);
							in.number = 0;
							bc->push(in);
						} else {
							last->first->compile(machine, bc);
						}
						bc->push(OP_EXIT);
						return;
					}

					ifcall(car) {
						if (last->first == NULL) throw "call to car requires one argument";
						last->first->compile(machine, bc);
						bc->push(OP_CAR);
						return;
					}

					ifcall(cdr) {
						if (last->first == NULL) throw "call to cdr requires one argument";
						last->first->compile(machine, bc);
						bc->push(OP_CDR);
						return;
					}

					ifcall(repl) {
						bc->push(OP_REPL);
						return;
					}

					ifcall(=) {
						if (last->first == NULL || last->last->first == NULL) throw "call to = requires two arguments";
						last->first->compile(machine, bc);
						last->last->first->compile(machine, bc);
						bc->push(OP_EQUAL);
						return;
					}

					ifcall(intern) {
						if (last->first == NULL) throw "call to intern requires an argument";
						last->first->compile(machine, bc);
						bc->push(OP_INTERN);
						return;
					}
				}

				int arg_count = 0;
				for (auto *arg = last; arg->type == value::list && arg->first != NULL; arg = arg->last) {
					arg->first->compile(machine, bc);
					arg_count++;
				}

				const char* callname = first->to_string().c_str();
				BIN_CALL("+", OP_ADD);
				BIN_CALL("-", OP_SUB);
				BIN_CALL("*", OP_MUL);
				BIN_CALL("/", OP_DIV);
				BIN_CALL("%", OP_MOD);
				BIN_CALL("and", OP_AND);
				BIN_CALL("or", OP_OR);
				UNARY_CALL("not", OP_NOT);

				first->compile(machine, bc);
				auto call = vm::Instruction(OP_CALL);
				call.whole = arg_count;
				bc->push(call);
			} break;


		case value::string:
			bc->push(vm::Instruction(OP_PUSH_STR, string));
			return;

		case value::number:
			bc->push(vm::Instruction(OP_PUSH_NUM, number));
			return;

		case value::procedure:
			bc->push(vm::Instruction(OP_NOP));
			break;

		case value::ident: {
				auto inst = vm::Instruction(OP_PUSH_LOOKUP);
				inst.string = string;
				if (!strcmp(string, "nil")) inst.opcode = OP_PUSH_NIL;
				bc->push(inst);
			}; break;

		case value::keyword: {
				 auto inst = vm::Instruction(OP_PUSH_RAW);
				 inst.object = this;
				 bc->push(inst);
			 }; break;
	}
}




void Object::compile_quote(vm::Machine* m, vm::Bytecode* bc) {
	// std::cout << "quote: " << to_string() << "\n";
	if (type == value::ident) {
		vm::Instruction inst(OP_PUSH_IDENT);
		inst.string = string;
		bc->push(inst);
		return;
	}

	if (type == value::string) {
		vm::Instruction inst(OP_PUSH_STR);
		inst.string = string;
		bc->push(inst);
		return;
	}

	if (type == value::nil) {
		vm::Instruction inst(OP_PUSH_NIL);
		bc->push(inst);
		return;
	}

	if (type == value::keyword) {
		vm::Instruction inst(OP_PUSH_RAW);
		inst.object = this;
		bc->push(inst);
		return;
	}

	if (type == value::number) {
		vm::Instruction inst(OP_PUSH_NUM);
		inst.number = number;
		bc->push(inst);
		return;
	}
	if (type == value::number) {
		vm::Instruction inst(OP_PUSH_NUM);
		inst.number = number;
		bc->push(inst);
		return;
	}

	if (type == value::list) {
		vm::Instruction inst(OP_PUSH_RAW);
		inst.object = this;
		bc->push(inst);
		return;
	}
}

bool Object::is_true() {
	return type != value::nil;
}


size_t Object::length() {
	size_t len = 0;

	Object* curr = this;
	while (curr->last != NULL) {
		if (curr->first != NULL) len++;
		curr = curr->last;
	}
	return len;
}

Object* Object::operator[](int index) {
	size_t len = 0;
	Object* curr = this;
	while (curr->last != NULL) {
		if (len == index) return curr->first;
		if (curr->first != NULL) len++;
		curr = curr->last;
	}
	return NULL;
}

