(def (fact n)
  (if (zero? n)
    1
    (* n (fact (- n 1)))))

(def (fib n)
  (if (< n 2)
    n
    (+ (fib (- n 1)) (fib (- n 2)))))

(print (fib 10))
