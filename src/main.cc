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
#include <state.hh>
#include <map>
#include <scope.hh>
#include <thread>
#include <value.hh>
#include <gc/gc.h>

// #include <gc/gc_cpp.h>
#define EXIT_FILE_ERROR 1
#define STDIN_READ_SIZE 100

static void xfinalizer(GC_PTR obj, GC_PTR x) {
	printf("%p collected\n", obj);
}

void* xmalloc(size_t size) {
	void* ptr = GC_MALLOC(size);
	// GC_register_finalizer(ptr, xfinalizer, 0, 0, 0);
	return ptr;
}

void* operator new(size_t size) {
	return xmalloc(size);
}
void* operator new[](size_t size) {
	return xmalloc(size);
}

#ifdef __GLIBC__
#define _NOEXCEPT _GLIBCXX_USE_NOEXCEPT
#endif
void operator delete(void* ptr) _NOEXCEPT {
	GC_FREE(ptr);
}
void operator delete[](void* ptr) _NOEXCEPT {
	GC_FREE(ptr);
}



void gc_thread() {
	while (true) {
		sleep(1);
		GC_gcollect();
	}
}

int main(int argc, char** argv) {
	//std::thread gcthread(gc_thread);
	GC_init();
	GC_enable_incremental();

	auto *state = new State();

	if (argc <= 1) {
		if (isatty(fileno(stdin))) {
			state->run_repl();
		} else {
			std::string contents;
			char buffer[STDIN_READ_SIZE];
			while (fgets(buffer, STDIN_READ_SIZE, stdin)) {
				contents += buffer;
			}
			state->eval(const_cast<char*>(contents.c_str()));
		}
		delete state;
		exit(0);
	}
	// if there was a file argument, evaluate that instead
	char* filepath = argv[1];
	state->eval_file(filepath);

	GC_gcollect();


	delete state;

	return 0;
}


