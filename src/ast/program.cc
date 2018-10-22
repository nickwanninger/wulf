#include <ast.hh>

using namespace ast;

void Program::push_node(Node* n) {
	nodes.push_back(n);
}

std::string Program::to_string() {
	return NULL;
}
