(defmacro my-set (name val)
  (list 1 2))


(print (macroexpand '(my-set hello 1)))

