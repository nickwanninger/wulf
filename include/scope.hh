#ifndef __SCOPE_HH
#define __SCOPE_HH

#include <wulf.hh>
#include <unordered_map>
#include <specialforms.hh>


NSCLASS(value, Object);

namespace scope {
	class Scope {
		private:
			std::unordered_map<std::string, value::Object> bindings;
		public:

			int index = 0;
			/*
			 * a scope can have a parent.
			 */
			Scope* parent = NULL;

			Scope* root = NULL;
			/*
			 * constructors
			 */
			Scope();
			Scope(Scope*);
			/*
			 * spawn a child for the scope
			 */
			Scope* spawn_child();
			value::Object find(std::string);
			void set(std::string, value::Object);
			void set(const char*, value::Object);
			void set(const char* name, specialformfn);
			void set(const char*, double);
			void install_default_bindings();
	};
}

#endif
