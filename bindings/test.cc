#include <bindings.hh>
#include <stdio.h>
#include <stdlib.h>
#define GC_THREADS
#include <gc/gc_cpp.h>
#include <thread>

const char* type_name = "my-type";

wulf_binding(make_custom) {
	value::Object *obj = new value::Object();
	obj->type = value::custom;
	obj->type_name = type_name;
	obj->payload = (void*)type_name;
	return obj;
}


wulf_binding(wulf_malloc) {
	if (argc != 1) throw "malloc requires on argument";
	if (argv[0]->type != value::number) throw "malloc requires a number (size)";
	value::Object *obj = new value::Object();
	obj->type = value::custom;
	obj->type_name = "buffer";
	obj->payload = GC_MALLOC((long)argv[0]->number);
	return obj;
}

void eval_thread_function(State* state, scope::Scope* sc, value::Object* obj) {
	struct GC_stack_base base;
	GC_get_stack_base(&base);
	GC_register_my_thread(&base);
	try {
		auto *machine = new vm::Machine();
		machine->eval(obj, sc);
		delete machine;
	} catch (const char* err) {
		std::cout << "THREAD ERROR: " << err << "\n";
	}
	GC_unregister_my_thread();
}

wulf_binding(eval_thread) {
	if (argc != 1) throw "eval-thread requires one argument";
	std::thread *thread = new std::thread(eval_thread_function, state, scope, argv[0]);
	value::Object *obj = new value::Object();
	obj->type = value::custom;
	obj->type_name = "thread";
	obj->payload = (void*)thread;
	return obj;
}

wulf_binding(thread_join) {
	if (argc != 1) throw "join requires one argument";
	if (argv[0]->type == value::custom && !strcmp(argv[0]->type_name, "thread")) {
		std::thread *thread = (std::thread*)argv[0]->payload;
		thread->join();
	} else {
		throw "join requires a thread object";
	}

	return new value::Object();
}
