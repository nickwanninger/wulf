#ifndef __SCOPE_HH
#define __SCOPE_HH

#include <map>
#include <value.hh>

namespace value {
	class Value;
}


namespace scope {
	class Scope {
		private:
			std::map<std::string, value::Value*> bindings;
		public:
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
			value::Value* find(std::string);
			void set(std::string, value::Value*);
	};
}

#endif
