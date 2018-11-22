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
#ifndef __SCOPE_HH
#define __SCOPE_HH

#include <wulf.hh>
#include <list>
#include <value.hh>
#include <map>


namespace scope {


	class Bucket {
		public:
			std::string key;
			value::obj val;
			Bucket* next;
	};

	class Valmap {
		private:
			long bucketsize;
			Bucket** buckets;
		public:
			Valmap();
			Valmap(long);
			bool contains(const std::string&);
			value::obj operator[](const std::string);
			Bucket* getbucket(const std::string);
	};

	class Scope {
		private:
			Valmap bindings;
		public:

			int index = 0;
			/*
			 * a scope can have a parent.
			 */
			Scope* parent = NULL;

			Scope* root = NULL;
			Scope* returning_scope = NULL;
			/*
			 * constructors
			 */
			Scope();
			Scope(Scope*);
			/*
			 * spawn a child for the scope
			 */
			Scope* spawn_child();
			value::obj find(std::string);
			Bucket* find_bucket(std::string);

			std::string to_string();
			void set(std::string, value::obj);
			void set(const char*, value::obj);
			void set(const char*, double);
			void install_default_bindings();
	};
}

#endif
