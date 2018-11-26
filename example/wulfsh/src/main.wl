(print "shell:")

(def wulfsh-so (dlopen "./bindings.so"))

(def sh-eval (dlfunc wulfsh-so "sh_eval"))

(defmacro (sh :rest args)
  `(sh-eval ,@(map str args)))
