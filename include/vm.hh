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
#ifndef __VM_HH
#define __VM_HH

#include <stdlib.h>
#include <vector>
#include <value.hh>
#include <opcode.hh>
#include <state.hh>
#include <stack>
#include <map>
#include <macro.hh>

typedef value::Object* stackval;

class State;
namespace vm {


	/*
	 */
	class Instruction {
		public:
			opcode_t opcode;
			union {
				double number;
				long long whole;
				char* string;
				value::Object* object;
				void *ptr;
			};
			Instruction(opcode_t);
			Instruction(opcode_t, double);
			Instruction(opcode_t, char*);
			Instruction(opcode_t, const char*);
			std::string to_string();
	};


	enum BC_Type {
		bc_normal,
		bc_binding,
	};
	class Bytecode {
		public:
			BC_Type type = bc_normal;
			const char* name;
			value::bind_func_t binding;
			std::vector<Instruction> instructions;
			value::Object *lambda; // a pointer to the definition for stringification
			Instruction & push(Instruction);
	};

	class Stack {
		private:
			int stack_block_size;
			int block_count;
			stackval** blocks;
			void resize(long);
		public:
			long index = 0;
			long stacksize;
			Stack();
			~Stack();
			void dump();
			long push(stackval);
			void set(long, stackval);
			stackval get(long);
			stackval& ref(long);
			stackval pop();
	};

	typedef struct {
		vm::Bytecode bc;
		long long pc;
		long long sp;
	} bytecode_stack_obj_t;

	class Machine {
		private:
		public:
			value::Object* nilval;
			value::Object* trueval;
			std::map<std::string, macro::Expansion*> macros;
			bool debug = false;
			Stack* stack;
			State* state;
			Machine();
			void eval(Bytecode, scope::Scope*);
			value::Object *eval(value::Object*, scope::Scope*);
			void handle_syscall(std::stack<bytecode_stack_obj_t>&, scope::Scope*&, long long&, vm::Instruction&, int, value::Object*);
	};

}

#endif
