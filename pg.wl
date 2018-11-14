(def (reduce f i xs)
  (if (nil? xs)
    i
    (reduce f (f i (car xs)) (cdr xs))))

(def (first l) (syscall 19 l))
(def (rest l) (syscall 20 l))

(def ($ a f b) (f a b))


;; recursive factorial
(def (fact n)
  (if (<= n 0)
    1
    (* n (fact (- n 1)))))

;; recursive fibonacci
(def (fib n)
  (if (< n 2)
    n
    (+ (fib (- n 1)) (fib (- n 2)))))




(def (sum f a b step)
  (if (> a b)
    0
    (+ (f a)
       (sum f (step a) b step))))

(def (cube x) (* x x x))

(def (sum-cubes a b)
  (sum cube a b inc))


(def (pi-sum a b)
  (if (> a b)
    0
    (+ (/ 1.0 (* a (+ a 2))) (pi-sum (+ a 4) b))))

(def (approx-pi) (* 8 (pi-sum 1 10000)))

(def (integral f a b dx)
  (* (sum f
          (+ a (/ dx 2.0))
          b
          (fn x (+ x dx)))
     dx))

(defmacro (when c v)
  `(if ,c ,v))


(print (map (fn x
              (cond ((> x 0) (list 'gt x))
                    ((< x 0) (list 'lt x))))
            (range -5 6)))
