/*
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

#include <ast.hh>

using namespace ast;

void List::push_node(Node *n) {
	args.push_back(n);
}

std::string List::to_string() {
	std::ostringstream os;
	os << "(";

	for (int i = 0; i < args.size(); i++) {
		auto& arg = args[i];
		os << arg->to_string();
		os << " ";
	}
	os << ")";
	return os.str();
}
