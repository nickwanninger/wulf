;(def (fact n)
;  (if (zero? n)
;    1
;    (* n (fact (- n 1)))))
;
;(def (fib n)
;  (if (< n 2)
;    n
;    (+ (fib (- n 1)) (fib (- n 2)))))


(def (len list)
  (if (nil? list)
    0
    (+ 1 (len (cdr list)))))


(def (foo a) (fn b a))


(print ((foo 1) 2))
; (print (len '(1 2 3 nil)))



