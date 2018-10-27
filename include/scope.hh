#ifndef __SCOPE_HH
#define __SCOPE_HH

#include <wulf.hh>
#include <list>
#include <specialforms.hh>
#include <value.hh>

#define BUCKET_SIZE 3

namespace scope {


	class Bucket {
		public:
			std::string key;
			value::Object* val;
	};

	class Valmap {
		private:
			std::list<Bucket> buckets[BUCKET_SIZE];
		public:
			bool contains(const std::string&);
			value::Object& operator[](const std::string);
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
