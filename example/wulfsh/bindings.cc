#include <wulf/bindings.hh>
#include <stdio.h>
#include <stdlib.h>


wulf_binding(sh_eval) {
	if (argc < 1) throw "sh-eval requires at least one argument";

	char **args = new char*[argc+1];
	args[argc] = NULL;

	for (int i = 0; i < argc; i++) {
		if (argv[i]->type != value::string) throw "sh-eval requires only strings";
		args[i] = argv[i]->string;
	}

	int pid = fork();
	if (pid == 0) {
		execvp(args[0], args);
		exit(0);
	}
	int status;
	waitpid(pid, &status, 0);

	value::obj exitstatus = value::newobj(value::number);
	exitstatus->number = WEXITSTATUS(status);

	return exitstatus;
}
