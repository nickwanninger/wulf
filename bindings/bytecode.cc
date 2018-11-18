#include <bindings.hh>
#include <vm.hh>


wulf_binding(bytecode_print) {
	if (argc != 1) throw "bytecode_print requires only one argument";

	if (argv[0]->type != value::procedure)
		throw "bytecode_print requires a procedure";
	if (argv[0]->code->type != vm::bc_normal)
		throw "bytecode_print requires a procedure written in wulf. no bindings etc..";
	for (auto inst : argv[0]->code->instructions) {
		std::cout << inst.to_string() << "\n";
	}
	return new value::Object();
}
