#ifndef __SCOPE_HH
#define __SCOPE_HH

#include <wulf.hh>
#include <map>

NSCLASS(value, Value)

	namespace scope {

		class Binding {
			public:
				Binding(value::Value*);
				Binding();
				value::Value* val;
		};


		class Scope {
			private:
				std::map<std::string, Binding*> bindings;
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
