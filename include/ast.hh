#ifndef __AST_HH__
#define __AST_HH__

#include <wulf.hh>

namespace ast {

	class Node {
		public:
			bool evaluated = false;
			// virtual methods for a node
			// all sub classes must implement these
			// (kinda like an interface, I guess?)
			virtual std::string to_string() = 0;
	};

	/*
	 * a Program represents a program's parse state,
	 * and has a listing of top level nodes
	 */
	class Program : public Node {
		private:
			std::vector<Node*> nodes;
		public:
			void push_node(Node*);
			std::string to_string();
	};

	class List : public Node {
		private:
			std::vector<Node*> args;
		public:
			void push_node(Node*);
			std::string to_string();
	};

}

#endif
