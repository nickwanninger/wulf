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


(def (first l) (syscall 19 l))
(def (rest l) (syscall 20 l))

(def ($ a f b) (f a b))

(def (* :rest args)
  (reduce (fn (a b) (syscall 13 (list a b))) 1 args))
(def (+ :rest args)
  (reduce (fn (a b) (syscall 11 (list a b))) 0 args))

; define gt because the language only defines lt
(def (> a b)
  (if (not (or (< a b) (= a b)))
    t nil))

(def (>= a b)
  (not (< a b)))

(def (<= a b)
  (not (> a b)))


(def (append l1 l2)
  (if (nil? l1)
    l2
    (cons (first l1)
          (append (rest l1) l2))))

(def (reverse l)
  (if (nil? l)
    '()
    (append (reverse (rest l)) (list (first l)))))


(def (abs x) (if (> x 0) x (- 0 x)))
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
    (cons (f (first l))
          (map f (rest l)))))


(def (each l f)
  (if (not (nil? l))
    (do
      (f (first l))
      (each (rest l) f))))


(def (prins :rest args)
  (do
    (each args (fn a (do (puts a) (puts " "))))
    (puts "\n")))

(def (range a b)
  (if (< a b) (cons a (range (inc a) b))))

(def (zero-to a) (range 0 a))

(def pi 3.14159265359)

(def (permute n r)
  (/ (fact n)
     (fact (- n r))))


(def (choose n r)
  (/ (fact n)
     (* (fact r) (fact (- n r)))))



(def (zip l1 l2)
  (if (not (or (nil? l1) (nil? l2)))
    (cons (list (first l1) (first l2))
          (zip (rest l1) (rest l2)))))

(def (rng) (syscall 22 nil))

;; 244
(def (makeaccount balance)
  (fn (amount)
    (if (>= balance amount)
      (do
          balance)
      "Insufficient funds")))

; calling account-1 will withdraw from the state
; and return the new balance
;(def A1 (make-account 100))
;(def A2 (make-account 100))

