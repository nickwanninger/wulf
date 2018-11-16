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


(def (second x) (car (cdr x)))

(def (ffirst x) (first (first x)))

;;; define the fixed-point y-combinator
(def Y                ; (Y f) = (g g) where
  (fn (f)             ;         (g g) = (f  (lambda a (apply (g g) a)))
    ((fn (g) (g g))   ; (Y f) ==        (f  (lambda a (apply (Y f) a)))
     (fn (g)
       (f (fn (:rest a) (apply (g g) a)))))))

(def fib1
  (Y (fn (f)
       (fn (x)
         (if (< x 2)
             x
             (+ (f (- x 1)) (f (- x 2))))))))

; tail-recursive Fibonacci
(def Yfib
  (fn (x)
    ((Y (fn (f)
      (fn (x a b)
        (if (< x 1)
          a
          (f (- x 1) b (+ a b))))))
     x 0 1)))

(def (fib-iter a b count)
  (if (= count 0)
    b
    (fib-iter (+ a b) a (- count 1))))

; (def (fib n) (fib-iter 1 0 n))


(def (thing n)
  (do
    (print n)
    (thing (inc n))))


(thing 0)


