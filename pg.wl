#!./wulf
(def (reduce f i xs)
  (if (nil? xs)
    i
    (reduce f (f i (car xs)) (cdr xs))))
;;; define n-arg math ops with reduce
;(def (+ :rest args)
;  (reduce (fn (a b) (syscall 11 (list a b))) 0 args))
;;; recursive, smart subtraction
;(def (- :rest args)
;  (do
;    (if (nil? (car args))
;      (die "subtraction requires at least one argument"))
;    (if (nil? (cdr args))
;      (- 0 (car args))
;      (reduce
;        (fn (a b) (syscall 12 (list a b)))
;        (car args) (cdr args)))))

;;;; ----------------------------------------------


(def (inf a f b) (f a b))

(def (* :rest args)
  (reduce (fn (a b) (syscall 13 (list a b))) 1 args))
; define gt because the language only defines lt
(def (> a b)
  (if (not (or (< a b) (= a b)))
    t nil))
(def (>= a b)
  (not (< a b)))
(def (<= a b)
  (not (> a b)))


(def (abs x) (if (> x 0) x (- 0 x)))
;; recursive factorial
(def (fact n)
  (if (zero? n)
    1
    (* n (fact (- n 1)))))

;; recursive fibonacci
(def (fib n)
  (if (< n 2)
    n
    (+ (fib (- n 1)) (fib (- n 2)))))

;; find the length of any list
(def (len l)
  (if (nil? l)
    0
    (+ 1 (len (cdr l)))))


(def (pow b n)
  (if (= n 0)
    1
    (* b (pow b (- n 1)))))


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



(def (map f l)
  (if (not (nil? l))
    (cons (f (car l))
          (map f (cdr l)))))

(def (first l) (car l))
(def (rest l) (cdr l))

(def (each l f)
  (if (not (nil? l))
    (do
      (f (first l))
      (each (rest l) f))))

(def (range a b)
  (if (< a b) (cons a (range (inc a) b))))


(def pi 3.14159265359)

(print "hello")
