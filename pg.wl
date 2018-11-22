(def (reduce f i xs)
  (if (nil? xs)
    i
    (reduce f (f i (car xs)) (cdr xs))))

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
          (fn x (+ x dx))) dx))

;;; define the y-combinator
(def Y                ; (Y f) = (g g) where
  (fn (f)             ;         (g g) = (f  (lambda a (apply (g g) a)))
    ((fn (g) (g g))   ; (Y f) ==        (f  (lambda a (apply (Y f) a)))
     (fn (g)
       (f (fn (:rest a) (apply (g g) a)))))))

; tail-recursive Fibonacci
(def Yfib
  (fn (x)
    ((Y (fn (f)
      (fn (x a b)
        (if (< x 1)
          a
          (f (- x 1) b (+ a b))))))
     x 0 1)))

(def (loopthing id i)
  (do
    (print (list id i (Yfib i)))
    (loopthing id (inc i))))


(def (make-counter)
  (let ((c 0))
    (fn ()
      (do (set! c (inc c))
          c))))
