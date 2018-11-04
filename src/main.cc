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
#include <unordered_map>
#include <scope.hh>
#include <thread>
#include <value.hh>
#include <gc/gc.h>
#include <vm.hh>
#include <value.hh>
#include <parser.hh>
#include <scanner.hh>
#include <getopt.h>
#include <unistd.h>

#define EXIT_FILE_ERROR 1
#define STDIN_READ_SIZE 100


int main(int argc, char** argv) {
	GC_init();

	bool interactive = false;

	auto *state = new State();
	char opt;
	while ((opt = getopt(argc, argv, "i")) != -1) {
		switch (opt) {
		case 'i': interactive = true; break;
		default:
				fprintf(stderr, "Usage: %s [-i] [file]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if (optind >= argc) {
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


	std::cout << "here\n";
	// if there was a file argument, evaluate that instead
	char* filepath = argv[optind];
	state->eval_file(filepath);

	// if the user requested interactive mode, do that.
	if (interactive) {
		state->run_repl();
	}

	// run a garbage collection run
	GC_gcollect();

	delete state;

	return 0;
}




// garbage collection crap
static void xfinalizer(GC_PTR obj, GC_PTR x) {
	printf("%p collected\n", obj);
}

void* xmalloc(size_t size) {
	void* ptr = GC_MALLOC(size);
	// printf("allocated %lu bytes to %p\n", size, ptr);
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
