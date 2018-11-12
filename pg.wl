#!./wulf
(def (reduce f i xs)
  (if (nil? xs)
    i
    (reduce f (f i (car xs)) (cdr xs))))

(def (first l) (syscall 19 l))
(def (rest l) (syscall 20 l))

(def ($ a f b) (f a b))


(def (nth n l)
  ; return nil if the user asks for anything less than 0
  (if (< n 0) nil
  (if (zero? n)
    (first l)
    (nth (dec n) (rest l)))))

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

(def (zero-to a) (range 0 (inc a)))
(def (zero-upto a) (range 0 a))

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

(def (rand) (syscall 22 nil))

(def (rand-range l u)
  (+ (* (rand) (- u l)) l))

;; 244
(def (make-account balance)
  (fn amount
    (if (>= balance amount)
      (do
        (set! balance (- balance amount))
        balance)
      "Insufficient funds")))



; calling account-1 will withdraw from the state
;and return the new balance

(def (make-counter i)
  (fn ()
    (do
      (set! i (inc i))
      i)))


(def (repeat val n)
  (if (zero? n)
    nil
    (cons val (repeat val (dec n)))))

