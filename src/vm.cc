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
#include <vm.hh>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <sys/mman.h>
#include <gc/gc_cpp.h>
#include <stack>
#include <syscall.hh>


using namespace vm;

#define STACK_GROWTH_RATIO 2
#define STACK_BASE_SIZE 64
#define STACK_BREATHING_ROOM 20

// #define STACK_DEBUG

typedef stackval* stackblock;

// constructor
Stack::Stack() {
	stack_block_size = 32;
	stacksize = stack_block_size;
	block_count = 1;
	blocks = new stackval*[block_count];
	blocks[0] = new stackval[stack_block_size];
}

Stack::~Stack() {
	// delete all stack blocks
	for (int i = 0; i < block_count; i++) delete[] blocks[i];
	// delete the block vector
	delete[] blocks;
}

void Stack::set(long i, stackval val) {
	// don't let people be dumbos
	if (i < 0) throw "Attempt to write to negative stack index";
	ref(i) = val;
}

stackval Stack::get(long i) {
	if (i < 0) throw "attempt to read from negative stack index";
	return ref(i);
}
/*
 * ref returns a reference to the specific object in the stack
 * essentially, it is an abstraction around the math it would take
 * to pick a block and an offset
 */
stackval& Stack::ref(long i) {
	int blk = floor(i / stack_block_size);
	// resize the stack if needed
	if (i >= stacksize) {
		long newsize = (long)(stacksize * STACK_GROWTH_RATIO);
		resize(newsize);
	}
	int offset = i % stack_block_size;
	if (blocks[blk] == NULL)
		blocks[blk] = new stackval[stack_block_size];
	return blocks[blk][offset];
}


void Stack::resize(long newsize) {

	// std::cout << "resizing stack from " << stacksize << " to " << newsize << "\n";
	int new_block_count = fmax(ceil(newsize / stack_block_size), 1);
	if (newsize < stack_block_size) return;
	newsize = new_block_count * stack_block_size;
	auto* new_blocks = new stackval*[newsize];
	if (index >= newsize)
		throw "attempt to shrink stack below stack index";
	long copy_count = (stacksize < newsize) ? block_count : new_block_count;
	memcpy(new_blocks, blocks, copy_count * sizeof(stackval*));
	if (newsize > stacksize)
		for (int i = 0; i < new_block_count; i++)
			if (i >= block_count) {
				new_blocks[i] = NULL;
			}
	delete[] blocks;
#ifdef STACK_DEBUG
	std::cout << ((newsize > stacksize) ? "grew" : "shrunk");
	std::cout << " stack from " << stacksize << " to " << newsize;
	std::cout << " \t(" << (newsize * sizeof(newsize)) / 1024.0 / 1024.0 << " MB)";
	std::cout << "\n";
#endif
	blocks = new_blocks;
	block_count = new_block_count;
	stacksize = newsize;
}


long Stack::push(stackval val) {
	val = new value::Object(*val);
	//
	// std::cout << "push: " << val << "\n";
	set(index++, val);
	return index;
}

stackval Stack::pop() {
	if (index > 0) --index;
	auto val = get(index);
	// zero out the old location
	ref(index+1) = NULL;
	long shrink_size = stacksize / STACK_GROWTH_RATIO;
	if (index + STACK_BREATHING_ROOM <= shrink_size && shrink_size >= STACK_BASE_SIZE) {
		resize(stacksize / STACK_GROWTH_RATIO);
	}
	// std::cout << "pop: " << val << "\n";
	return val;
}

void Stack::dump() {
	for (int i = 0; i < index; i++) {
		printf("0x%06x  %s\n", i, get(i)->to_string().c_str());
	}
}


Instruction::Instruction(opcode_t op) {
	opcode = op;
}

Instruction::Instruction(opcode_t op, double num) {
	opcode = op;
	number = num;
}

Instruction::Instruction(opcode_t op, char* str) {
	opcode = op;
	string = str;
}

Instruction::Instruction(opcode_t op, const char* str) {
	opcode = op;
	string = ccharcopy(str);
}



#define OP_STRING(op) \
	case op:            \
		buf << #op

std::string Instruction::to_string() {
	std::ostringstream buf;


	switch (opcode) {
		OP_STRING(OP_NOP); break;
		OP_STRING(OP_PUSH_NIL); break;
		OP_STRING(OP_PUSH_NUM) << "\t" << number; break;
		OP_STRING(OP_PUSH_IDENT) << "\t" << string; break;
		OP_STRING(OP_PUSH_STR) << "\t'" << string << "'"; break;
		OP_STRING(OP_PUSH_LOOKUP) << "\t" << string; break;
		OP_STRING(OP_STORE_GLOBAL) << "\t" << string; break;
		OP_STRING(OP_STORE_LOCAL) << "\t" << string; break;
		// math ops
		OP_STRING(OP_ADD); break;
		OP_STRING(OP_SUB); break;
		OP_STRING(OP_MUL); break;
		OP_STRING(OP_DIV); break;
		OP_STRING(OP_MOD); break;
		OP_STRING(OP_LT); break;
		// logic operations
		OP_STRING(OP_AND); break;
		OP_STRING(OP_OR); break;
		OP_STRING(OP_NOT); break;

		OP_STRING(OP_CONS); break;
		OP_STRING(OP_CALL) << "\t" << whole; break;
		OP_STRING(OP_EXIT); break;
		OP_STRING(OP_INTERN); break;
		OP_STRING(OP_PUSH_RAW) << "\t" << object->to_string(); break;
		OP_STRING(OP_RETURN); break;
		OP_STRING(OP_JUMP) << "\t" << whole; break;
		OP_STRING(OP_JUMP_FALSE) << "\t" << whole; break;
		OP_STRING(OP_SYSCALL); break;
		OP_STRING(OP_BC_RETURN); break;
	}
	return buf.str();
}


Instruction & Bytecode::push(Instruction i) {
	instructions.push_back(i);
	return instructions[instructions.size()-1];
}



Machine::Machine() {
	stack = new Stack();
	nilval = new value::Object();
	trueval = new value::Object("t");
}


// provide a quick wrapper for evaluating an object with some scope
//   this is used internally for the macro expansion system
value::Object* vm::Machine::eval(value::Object obj, scope::Scope* sc) {
	vm::Bytecode bc;

	obj.compile(this, sc, &bc);

	eval(bc, sc);

	return stack->pop();
}

#define EVAL_DEBUG

#define VAL_TRUE(val) ((val)->type != value::nil)

long long call_count = 0;


void Machine::eval(Bytecode bc, scope::Scope* calling_scope) {
	scope::Scope* sc = calling_scope;
	size_t instruction_count = bc.instructions.size();

	std::stack<bytecode_stack_obj_t> bc_stk;

	bc_stk.push({bc, 0, 0});




	trueval->type = value::ident;

	while (bc_stk.top().pc < bc_stk.top().bc.instructions.size()) {
		long long & pc = bc_stk.top().pc;


		vm::Instruction & in = bc_stk.top().bc.instructions[pc];

		// std::cout << in.to_string() << "\n";
		if (debug) {
			system("clear");
			printf("in: %s\n", in.to_string().c_str());
			printf("pc: %llu\n", pc);
			printf("Stack:\n");
			for (int i = 0; i < stack->index; i++) {
				printf("0x%04x: %s\n", i, stack->get(i)->to_string().c_str());
			}
			std::cin.ignore();
		}

		switch (in.opcode) {

			case OP_PUSH_NIL: {
					stack->push(nilval);
					pc++;
				}; break;

			case OP_PUSH_NUM: {
					auto *val = new value::Object(in.number);
					stack->push(val);
					pc++;
				} break;

			case OP_PUSH_IDENT: {
					auto *ident = new value::Object(in.string);
					ident->type = value::ident;
					stack->push(ident);
					pc++;
				} break;

			case OP_PUSH_STR: {
					auto *val = new value::Object(in.string);
					val->type = value::string;
					stack->push(val);
					pc++;
				} break;

			case OP_PUSH_LIST: {
					auto *val = new value::Object();
					val->type = value::list;
					stack->push(val);
					pc++;
				} break;


			case OP_INTERN: {
						auto *str = stack->pop();
						if (str->type != value::string) throw "attempt to intern non-string";
						auto *kw = new value::Object();
						kw->type = value::keyword;
						kw->string = new char[sizeof(str->string)+2];
						strcpy(kw->string+1, str->string);
						kw->string[0] = ':';
						stack->push(kw);
						pc++;
						break;
					}


			case OP_SKIP:
					stack->pop();
					pc++;
					break;


			case OP_AND: {
					auto b = stack->pop();
					auto a = stack->pop();
					if (!(VAL_TRUE(a) && VAL_TRUE(b))){
						stack->push(nilval);
					} else {
						stack->push(trueval);
					}
					pc++;
				} break;
			case OP_OR: {
					auto b = stack->pop();
					auto a = stack->pop();
					if (!(VAL_TRUE(a) || VAL_TRUE(b))){
						stack->push(nilval);
					} else {
						stack->push(trueval);
					}
					pc++;
				} break;




			case OP_EXIT: {
					auto code = stack->pop();
					if (code->type == value::number) exit((int)code->number);
					std::cerr << code->to_string() << "\n";
					exit(1);
					pc++;
				}; break;

			case OP_NOP:
				pc++;
				break;

			case OP_PUSH_RAW: {
					auto *obj = in.object;
					if (obj->type == value::procedure) {
						obj->defining_scope = sc;
					}
					stack->push(obj);
					pc++;
				}; break;

			case OP_PUSH_LOOKUP: {
					value::Object *val = sc->find(in.string);
					stack->push(val);
					pc++;
				}; break;

			case OP_STORE_GLOBAL: {
					value::Object *val = stack->pop();
					sc->root->set(in.string, val);
					stack->push(val);
					pc++;
				}; break;


			case OP_STORE_LOCAL: {
					auto val = stack->pop();
					scope::Bucket *buck = sc->find_bucket(std::string(in.string));
					if (buck == NULL) {
						sc->set(in.string, val);
					} else {
						buck->val = val;
					}
					stack->push(val);
					pc++;
				}; break;

			case OP_CALL: {
					int i;
					bytecode_stack_obj_t new_call;
					auto callable = stack->pop();
					// where the "frame" starts for this call. this will be used to jump back to and push
					// the return value when returning
					new_call.sp = stack->index;
					if (callable->type != value::procedure) {
						std::cout << "CALLABLE: " << callable->to_string() << "\n";
						throw "attempt to call non-procedure";
					}



					if (callable->code->type == vm::bc_binding) {
						int argc = in.whole;
						value::Object **argv = new value::Object*[argc];
						for (int i = argc-1; i >= 0; i--) {
							argv[i] = stack->pop();
						}
						value::Object *res = callable->code->binding(argc, argv, state);
						stack->push(res);
					} else if (callable->code->type == vm::bc_normal) {
						// printf("defining scope: %p\n", (void*) callable.defining_scope);
						int passedc = in.whole;
						std::vector<value::Argument> argnames = *callable->args;
						std::vector<value::Object*> arglist;

						// pop off the args in the right order.
						for (i = 0; i < passedc; i++) {
							auto *arg = stack->pop();
							arglist.insert(arglist.begin(), arg);
						}

						auto *newscope = callable->defining_scope->spawn_child();
						newscope->returning_scope = sc;
						// expand the arguments into the scope
						value::argument_scope_expand(argnames, arglist, newscope);
						sc = newscope;
						vm::Bytecode lambda_bc = *(callable->code);
						new_call.bc = lambda_bc;
						new_call.pc = 0;
						bc_stk.push(new_call);
					} else throw "unknown callable type";
					pc++;
				}; break;


			case OP_RETURN: {
					auto *parent = sc->returning_scope;
					if (parent != NULL) {
						sc = parent;
					} else {
						throw "no returning scope";
					}
					auto top = bc_stk.top();
					auto val = stack->pop();
					// stack->index = top.sp-1;
					stack->push(val);
					if (bc_stk.size() != 1)	bc_stk.pop();
					pc++;
				}; break;


			// BC_RETURN only pops the bytecode.
			//   this is used at the end of an eval
			case OP_BC_RETURN: {
					if (bc_stk.size() != 1) {
						bc_stk.pop();
					} else {
						throw "no returning bytecode for BC_RETURN";
					}
					pc++;
				}; break;


			case OP_CONS: {

						auto lst = stack->pop();
						auto val = stack->pop();

						if (lst->type != value::list) {
							if (lst->type == value::nil) {
								lst->type = value::list;
							} else {
								throw "attempt to cons to non-list";
							}
						}
						auto *newlist = new value::Object();
						newlist->type = value::list;
						newlist->first = val;
						newlist->last = lst;
						stack->push(newlist);
						pc++;
					}; break;

			case OP_SYSCALL: {
					auto arg2 = stack->pop();
					auto arg1 = stack->pop();


					// std::cout << "(syscall " << arg1.to_string() << " " << arg2.to_string() << ")\n";

					if (arg1->type != value::number) throw "attempt to call syscall with non-number as first argument";
					int syscalli = arg1->number;

					handle_syscall(bc_stk, sc, pc, in, syscalli, arg2);


					pc++;
				}; break;

			case OP_NOT: {
					auto *arg = stack->pop();
					if (arg->type == value::nil) {
						stack->push(trueval);
					} else {
						stack->push(nilval);
					}
					pc++;
				}; break;

			case OP_JUMP: {
					pc = in.whole;
				}; break;

			case OP_JUMP_FALSE: {
					auto *arg = stack->pop();
					if (arg->type == value::nil)
						pc = in.whole;
					else pc++;
				}; break;



			default:
				std::ostringstream err;
				err << "Unimplemented Instruction";
				err << " (0x" << std::hex << int(in.opcode) << ")  ";
				err << in.to_string();
				throw err.str();
		}


	}


	return;
}


std::string eval_shell_cmd(char* cmdc) {
	std::string cmd = cmdc;
	cmd += " 2>&1";
	std::string data;

	const int max_buffer = 256;
	char buffer[max_buffer];

	FILE* stream = popen(cmd.c_str(), "r");
	if (stream) {
		while (!feof(stream)) {
			if (fgets(buffer, max_buffer, stream) != NULL) data += buffer;
		}
		pclose(stream);
	}
	return data;
}


#define OP_BINARY(op) {                                             \
	auto b = arg[1];                                                  \
	auto a = arg[0];                                                  \
	if (a->type == b->type && a->type == value::number) {             \
		stack->push(value::Object(a->number op b->number));             \
	} else {                                                          \
		throw "attempt to perform math on two non-number values";       \
	}                                                                 \
};
void vm::Machine::handle_syscall(
		std::stack<bytecode_stack_obj_t> & bc_stk, \
		scope::Scope* & sc,
		long long & pc,
		vm::Instruction & inst,
		int syscalli,
		value::Object *arg) {
	// 0 = exit
	//
	switch (syscalli) {
		case (SYS_EXIT): {
			if (arg->type == value::number) {
				exit(arg->number);
			} else {
				std::cout << arg->to_string(true) << "\n";
				exit(1);
			}
		}; break;

		case SYS_EVAL: {
			Bytecode evalbc;
			arg->compile(this, sc, &evalbc);
			evalbc.push(OP_BC_RETURN);
			bc_stk.push({evalbc, 0, stack->index});
		}; break;

		case SYS_MACROEXPAND: {
				if (arg->type != value::list) throw "unable to macroexpand a non-list";
				if (arg->length() == 0) throw "unble to macroexpand empty list";
				if (arg->first->type != value::ident) throw "unable to macroexpand list where first element is not an ident";
				const char* callname = arg->first->to_string().c_str();

				if (macros.count(arg->first->to_string())) {
					// construct an argument list for the expansion
					std::vector<value::Object*> args;
					int len = arg->length();
					for (int i = 1; i < len; i++) {
						args.push_back(arg->operator[](i));
					}
					auto macro = macros[callname];
					// expand and compile the macro
					auto *expansion = macro.expand(this, args, sc);
					stack->push(expansion);
					return;
				} else throw "unable to find macro to expand";
		}; break;

		case SYS_PRINT: {
			std::cout << arg->to_string(true);
		}; break;

		case SYS_LOAD: {
			if (arg->type != value::string) throw "syscall load requires a string filepath";
			bool repls = state->repl;
			state->repl = false;
			char* path = ccharcopy(arg->to_string(true).c_str());
			state->eval_file(path);
			state->repl = repls;
		}; break;

		case SYS_TYPE: {
			auto val = arg;
			auto *type_kw = new value::Object(value::keyword);
			type_kw->type = value::keyword;
			using namespace value;
			#define TYPEOFVALUE(x) if (val->type == x) {type_kw->string = ccharcopy(":"#x);}
			TYPEOFVALUE(list);
			TYPEOFVALUE(ident);
			TYPEOFVALUE(string);
			TYPEOFVALUE(number);
			TYPEOFVALUE(procedure);
			TYPEOFVALUE(keyword);
			TYPEOFVALUE(nil);
			stack->push(type_kw);
		}; break;

		case SYS_SHELL: {
			if (arg->type != value::string) throw "shell systemcall requires string command to be executed with `sh -c '...'`";
			std::string data = eval_shell_cmd(arg->string);
			value::Object *obj = new value::Object(value::string);
			obj->string = ccharcopy(data.c_str());
			stack->push(obj);
		}; break;


		case SYS_CAR: {
			auto lst = arg;
			if (lst->type != value::list) {
				if (lst->type == value::nil) {
					stack->push(nilval);
				} else {
					throw "attempt to car non-list";
				}
			} else {
				stack->push(new value::Object(*lst->first));
			}
		}; break;

		case SYS_CDR: {
			auto lst = arg;
			if (lst->type != value::list) {
				if (lst->type == value::nil) {
					stack->push(nilval);
				} else {
					throw "attempt to cdr non-list";
				}
			} else {
				if (lst->last == NULL) {
					stack->push(nilval);
				} else {
					if (lst->last->length() == 0) {
						stack->push(nilval);
					} else {
						stack->push(lst->last);
					}
				}
			}
		}; break;

		case SYS_RNG: {
			double x = (double)rand()/(double)(RAND_MAX);
			auto *num = new value::Object(value::number);
			num->number = x;
			stack->push(num);
		}; break;

		case SYS_GC_COLLECT: {
			size_t start = GC_get_heap_size();
			GC_gcollect();
			size_t diff = GC_get_heap_size();
			auto *num = new value::Object(value::number);
			num->number = diff;
			stack->push(num);
		}; break;

		case SYS_CONS: {
			auto *val = arg->operator[](0);
			auto *lst = arg->operator[](1);
			if (lst->type != value::list) {
				if (lst->type == value::nil) {
					lst->type = value::list;
				} else {
					throw "attempt to cons to non-list";
				}
			}
			auto *newlist = new value::Object();
			newlist->type = value::list;
			newlist->first = val;
			newlist->last = lst;
			stack->push(newlist);
		}; break;



		case SYS_EQUAL: {
				auto *a = arg->operator[](0);
				auto *b = arg->operator[](1);
				if (a->type == b->type && a->type == value::number) {
					if (a->number == b->number) {
						stack->push(trueval);
					} else {
						stack->push(nilval);
					}
					return;
				}
				// special case nil and list comparison
				// todo: wrap this all in an `==` overloaded operator
				if (a->type == value::list || b->type == value::list) {
					if (a->type == value::nil || b->type == value::nil) {
						auto the_list = a->type == value::list ? a : b;
						auto the_nil = a->type == value::nil ? a : b;
						if (the_list->length() != 0) {
							stack->push(nilval);
						}
					}
				} else if (a->type != b->type || a->to_string() != b->to_string()) {
					stack->push(nilval);
				} else {
					stack->push(trueval);
				}
			}; break;



		case SYS_LT: {
				auto *a1 = arg->operator[](0);
				auto *a2 = arg->operator[](1);
				if (a2->type != a1->type || a2->type != value::number) throw "attempt to compare two non-numbers";
				if (a1->number < a2->number) {
					stack->push(trueval);
				} else {
					stack->push(nilval);
				}
			}; break;



		case SYS_STRCONV: {
				value::Object *str_o = new value::Object();
				str_o->type = value::string;
				std::string s = arg->to_string(true);
				str_o->string = new char[s.size()+1];
				memcpy(str_o->string, s.c_str(), s.size() + 1);
				stack->push(str_o);
			}; break;

		case SYS_STRLEN: {
				stack->push(new value::Object((double)arg->to_string().size()));
			}; break;

		case SYS_STRREF: {
				stack->push(new value::Object((double)arg->to_string().size()));
			}; break;

		case SYS_STRSET: {
				stack->push(new value::Object((double)arg->to_string().size()));
			}; break;

		case SYS_STRCAT: {
				if (arg->operator[](0)->type != value::string || arg->operator[](0)->type != arg->operator[](1)->type) throw "string concatination requires two strings";
				auto s = arg->operator[](0)->to_string(true) + arg->operator[](1)->to_string(true);
				auto len = s.size();
				value::Object *str_o = new value::Object();
				str_o->type = value::string;
				str_o->string = new char[len+1];
				memcpy(str_o->string, s.c_str(), len+1);
				stack->push(str_o);
			}; break;


		default: {
			stack->push(nilval);
			std::cerr << "unknown syscall " << syscalli << ". ignoring and returning nil\n";
		}
	}
}
