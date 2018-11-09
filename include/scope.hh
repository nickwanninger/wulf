#ifndef __SCOPE_HH
#define __SCOPE_HH

#include <wulf.hh>
#include <list>
#include <specialforms.hh>
#include <value.hh>
#include <map>


namespace scope {


	class Bucket {
		public:
			std::string key;
			value::Object val;
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
			value::Object& operator[](const std::string);
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
			value::Object find(std::string);

			std::string to_string();
			void set(std::string, value::Object);
			void set(const char*, value::Object);
			void set(const char*, double);
			void install_default_bindings();
	};
}

#endif
