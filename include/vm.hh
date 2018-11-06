#ifndef __VM_HH
#define __VM_HH

#include <stdlib.h>
#include <vector>
#include <value.hh>
#include <opcode.hh>
#include <state.hh>

typedef value::Object stackval;


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
			};
			Instruction(opcode_t);
			Instruction(opcode_t, double);
			Instruction(opcode_t, char*);
			Instruction(opcode_t, const char*);
			std::string to_string();
	};

	class Bytecode {
		public:
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

	class Machine {
		private:
		public:
			bool debug = false;
			Stack* stack;
			State* state;
			Machine();
			void eval(Bytecode, scope::Scope*);
	};
}

#endif
