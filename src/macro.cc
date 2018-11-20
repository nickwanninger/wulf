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
#include <macro.hh>
#include <vm.hh>

using namespace macro;

// #define MACRO_DEBUG
value::Object *macro::Expansion::expand(vm::Machine* machine, std::vector<value::Object*> arg_objs, scope::Scope* calling_scope) {

#ifdef MACRO_DEBUG
	std::cout << ";\tExpanding macro " << this->name << "\n";
	std::cout << ";\targs expected: (";
	for (auto a : args) {
		if (a.type == value::rest) std::cout << ":rest ";
		std::cout << a.name << " ";
	}
	std::cout << "\b)\n";
	std::cout << ";\targs passed: (";
	for (auto a : arg_objs) std::cout << a->to_string() << " ";
	std::cout << "\b)\n";

	std::cout << body << "\n";

	std::cout << ";\tbody: " << body->to_string() << "\n";
#endif

	scope::Scope *evaluation_scope = calling_scope->spawn_child();

	try {
	value::argument_scope_expand(args, arg_objs, evaluation_scope);
	} catch (const char* err) {
		std::cerr << "error in macro '" << name << "' expansion: " << err << "\n";
		exit(1);
	}
	value::Object *exp = machine->eval(body, evaluation_scope);
#ifdef MACRO_DEBUG
	std::cout << ";\tExpansion: " << exp->to_string() << "\n";
#endif
	return exp;
}
