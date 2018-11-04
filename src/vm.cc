#include <wulf.hh>
#include <vm.hh>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <sys/mman.h>
#include <gc/gc.h>
#include <stack>
#include <syscall.hh>
using namespace vm;

#define STACK_GROWTH_RATIO 2
#define STACK_BASE_SIZE 64
#define STACK_BREATHING_ROOM 20

#define STACK_DEBUG

typedef stackval* stackblock;

// constructor
Stack::Stack() {
	stack_block_size = getpagesize() / sizeof(stackval);
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
	if (i < 0) return value::Object();
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
	set(index++, val);
	return index;
}

stackval Stack::pop() {
	if (index > 0) --index;
	auto val = get(index);
	// zero out the old location
	ref(index+1) = value::Object();
	long shrink_size = stacksize / STACK_GROWTH_RATIO;
	if (index + STACK_BREATHING_ROOM <= shrink_size && shrink_size >= STACK_BASE_SIZE) {
		resize(stacksize / STACK_GROWTH_RATIO);
		GC_gcollect();
	}
	return val;
}

void Stack::dump() {
	for (int i = 0; i < index; i++) {
		printf("0x%06x  %s\n", i, get(i).to_string().c_str());
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
	string = (char*)str;
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
		// math ops
		OP_STRING(OP_ADD); break;
		OP_STRING(OP_SUB); break;
		OP_STRING(OP_MUL); break;
		OP_STRING(OP_DIV); break;
		OP_STRING(OP_MOD); break;
		// logic operations
		OP_STRING(OP_AND); break;
		OP_STRING(OP_OR); break;
		OP_STRING(OP_NOT); break;

		OP_STRING(OP_CALL) << "\t" << whole; break;
		OP_STRING(OP_CONS); break;
		OP_STRING(OP_EXIT); break;
		OP_STRING(OP_CAR); break;
		OP_STRING(OP_CDR); break;
		OP_STRING(OP_EQUAL); break;
		OP_STRING(OP_INTERN); break;
		OP_STRING(OP_PUSH_RAW) << "\t" << object->to_string(); break;
		OP_STRING(OP_RETURN); break;
		OP_STRING(OP_GOTO) << "\t" << whole; break;
		OP_STRING(OP_SYSCALL) << "\t" << object->to_string(); break;
	}
	return buf.str();
}


void Bytecode::push(Instruction i) {
	instructions.push_back(i);
}



Machine::Machine() {
	stack = new Stack();
}

#define OP_BINARY(op) {                                             \
	auto b = stack->pop();                                            \
	auto a = stack->pop();                                            \
	if (a.type == b.type && a.type == value::number) {                \
		stack->push(value::Object(a.number op b.number));               \
	} else {                                                          \
		throw "attempt to perform math on two non-number values";       \
	}                                                                 \
	pc++;                                                             \
} break;

#define EVAL_DEBUG

#define VAL_TRUE(val) ((val).type != value::nil)

long long call_count = 0;


typedef struct {
	vm::Bytecode bc;
	long long pc;
} bytecode_stack_obj_t;

void Machine::eval(Bytecode bc, scope::Scope* calling_scope) {
	scope::Scope* sc = calling_scope;
	size_t instruction_count = bc.instructions.size();

	std::stack<bytecode_stack_obj_t> bc_stk;

	bc_stk.push({bc, 0});

	while (bc_stk.top().pc < bc_stk.top().bc.instructions.size()) {
		long long & pc = bc_stk.top().pc;
		vm::Instruction & in = bc_stk.top().bc.instructions[pc];
		if (debug) {
			system("clear");
			printf("in: %s\n", in.to_string().c_str());
			printf("pc: %llu\n", pc);
			printf("Stack:\n");
			for (int i = 0; i < stack->index; i++) {
				printf("0x%04x: %s\n", i, stack->get(i).to_string().c_str());
			}
			std::cin.ignore();
		}

		switch (in.opcode) {

			case OP_PUSH_NIL:
				stack->push(value::Object());
				pc++;
				break;

			case OP_PUSH_NUM: {
					auto val = value::Object(in.number);
					stack->push(val);
					pc++;
				} break;

			case OP_PUSH_IDENT: {
					auto ident = value::Object(in.string);
					ident.type = value::ident;
					stack->push(ident);
					pc++;
				} break;

			case OP_PUSH_STR: {
					auto val = value::Object(in.string);
					val.type = value::string;
					stack->push(val);
					pc++;
				} break;

			case OP_PUSH_LIST: {
					auto val = value::Object();
					val.type = value::list;
					stack->push(val);
					pc++;
				} break;

			case OP_ADD: OP_BINARY(+);
			case OP_SUB: OP_BINARY(-);
			case OP_MUL: OP_BINARY(*);
			case OP_DIV: OP_BINARY(/);

			case OP_EQUAL: {
						auto a = stack->pop();
						auto b = stack->pop();
						if (a.type != b.type || a.to_string() != b.to_string()) {
							stack->push(value::Object(value::nil));
							pc++;
							break;
						}
						auto tr = value::Object("t");
						tr.type = value::ident;
						stack->push(tr);
						pc++;
						break;
					}
			case OP_INTERN: {
						auto str = stack->pop();
						if (str.type != value::string) throw "attempt to intern non-string";
						auto kw = value::Object();
						kw.type = value::keyword;
						kw.string = new char[sizeof(str.string)+2];
						strcpy(kw.string+1, str.string);
						kw.string[0] = ':';
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
						stack->push(value::Object());
					} else {
						auto t = value::Object("t");
						t.type = value::ident;
						stack->push(t);
					}
					pc++;
				} break;


			case OP_CONS: {
					auto val = stack->pop();
					auto lst = stack->pop();
					if (lst.type != value::list) {
						throw "attempt to cons to non-list";
					}
					auto newlist = value::Object();
					newlist.type = value::list;
					newlist.first = new value::Object(val);
					newlist.last = new value::Object(lst);
					stack->push(newlist);
					pc++;
				} break;


			case OP_EXIT: {
					auto code = stack->pop();
					if (code.type == value::number) exit((int)code.number);
					std::cerr << code.to_string() << "\n";
					exit(1);
					pc++;
				}; break;

			case OP_CAR: {
					auto lst = stack->pop();
					if (lst.type != value::list) throw "attempt to CAR a non-list";
					stack->push(*lst.first);
					pc++;
				}; break;
			case OP_CDR: {
					auto lst = stack->pop();
					if (lst.type != value::list) throw "attempt to CDR a non-list";
					stack->push(*lst.last);
					pc++;
				}; break;
			case OP_NOP:
				pc++;
				break;

			case OP_PUSH_RAW:
				stack->push(*in.object);
				pc++;
				break;

			case OP_PUSH_LOOKUP: {
					value::Object val = sc->find(in.string);
					stack->push(val);
					pc++;
				}; break;

			case OP_STORE_GLOBAL: {
					auto val = stack->pop();
					sc->root->set(in.string, val);
					stack->push(val);
					pc++;
				}; break;


			case OP_CALL: {
					// std::cout << "calling " << call_count++ << "\n";
					auto callable = stack->pop();
					if (callable.type != value::procedure) throw "attempt to call non-procedure";

					if (in.whole != callable.argc) {
						throw "invalid argument count to function call";
					}


					sc = sc->spawn_child();

					for (int i = callable.argc; i > 0; i--) {
						auto arg = stack->pop();
						sc->set(std::string(callable.argv[i-1]),arg);
					}

					bytecode_stack_obj_t new_call;
					vm::Bytecode lambda_bc = *(callable.code);
					new_call.bc = lambda_bc;
					new_call.pc = 0;
					bc_stk.push(new_call);
					pc++;
				}; break;

			case OP_RETURN: {
					auto *parent = sc->parent;
					if (parent == NULL) throw "attempt to return to NULL parent scope";
					sc = parent;
					bc_stk.pop();



					pc++;
				}; break;


			case OP_SYSCALL: {
					auto arg2 = stack->pop();
					auto arg1 = stack->pop();

					if (arg1.type != value::number) throw "attempt to call syscall with non-number as first argument";
					int syscalli = arg1.number;

					// 0 = exit
					if (syscalli == SYS_EXIT) {
						if (arg2.type == value::number) {
							exit(arg2.number);
						} else {
							std::cout << arg2.to_string() << "\n";
							exit(1);
						}
					}

					if (syscalli == SYS_PRINT) {
						std::cout << arg2.to_string();
					}

					if (syscalli == SYS_LOAD) {
						if (arg2.type != value::string) throw "syscall load requires a string filepath";
						bool repls = state->repl;
						state->repl = false;
						char* path = (char*)arg2.to_string().c_str();
						state->eval_file(path);
						state->repl = repls;
					}
					pc++;
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
