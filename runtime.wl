


(def wulf/copyright "
  Wulf Lisp Interpreter
  Copyright (C) 2018  Nick Wanninger

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.")

(def wulf/warranty "WIP")

(def (exit) (syscall 1 0))
(def (die n) (syscall 1 n))

(def (macroexpand exp) (syscall 3 exp))

(def (+ :rest a) (syscall 11 a))

(def (- :rest a) (syscall 12 a))

(def (* :rest a) (syscall 13 a))

(def (/ :rest a) (syscall 14 a))

(def (abs x) (if (> x 0) x (- 0 x)))

(def (pow b n)
  (if (= n 0)
    1
    (* b (pow b (- n 1)))))


(def (= a b) (syscall 25 (list a b)))
(def (< a b) (syscall 26 (list a b)))

(def (> a b) (not (or (< a b) (= a b))))
(def (>= a b) (not (< a b)))
(def (<= a b) (not (> a b)))

(def (cons a b) (syscall 24 (list a b)))
(def (eval stmt) (syscall 2 stmt))
(def (apply f a) (eval (cons f a)))

;; return a random number between zero and one
(def (rand) (syscall 22 nil))

;; return a random number between l and u
(def (rand-range l u)
  (+ (* (rand) (- u l)) l))


(def (let-helper/names args) (map car args))
(def (let-helper/vals args) (map (fn x (first (rest x))) args))

(defmacro (let vals-n-names :rest body)
  `((fn ,(let-helper/names vals-n-names)
      (do ,@body))
    ,@(let-helper/vals vals-n-names)))




(def (puts m) (syscall 7 m))
(def (print x) (do (puts x) (puts "\n")))
(def (prins :rest args)
  (do
    (each args (fn a (do (puts a) (puts " "))))
    (puts "\n")
    nil))

(def (list :rest l) l)

(def (type x) (syscall 9 x))
(def (sh cmd) (syscall 10 cmd))

(def (inc a) (+ a 1))
(def (dec a) (- a 1))

;; define the "truth" value
(def t 't)

(def (nil? x) (= x nil))
(def (true? x) (not (= x nil)))
(def (zero? x) (= x 0))

(def (ident? n) (= (type n) :ident))
(def (list? n) (= (type n) :list))
(def (string? n) (= (type n) :string))
(def (procedure? n) (= (type n) :procedure))
(def (keyword? n) (= (type n) :keyword))


;; convert any object v to a string
(def (str v) (syscall 31 v))
(def (string-length s) (syscall 27 s))
(def (string-concat s1 s2) (syscall 30 (list s1 s2)))


(def (car l) (syscall 19 l))
(def (cdr l) (syscall 20 l))

(def (first l) (syscall 19 l))
(def (rest l) (syscall 20 l))





;; find the length of any list
(def (len l)
  (if (nil? l)
    0
    (+ 1 (len (cdr l)))))



;; create a list of numbers a inclusive to b exclusive
(def (range a b)
  (if (< a b) (cons a (range (inc a) b))))
(def (zero-to a) (range 0 (inc a)))
(def (zero-upto a) (range 0 a))


;; return the nth item in a list l
(def (nth n l)
  ; return nil if the user asks for anything less than 0
  (if (< n 0) nil
  (if (zero? n)
    (first l)
    (nth (dec n) (rest l)))))


;; append two lists
(def (append l1 l2)
  (if (nil? l1)
    l2
    (cons (first l1)
          (append (rest l1) l2))))


;; reduce the function f over xs starting at i
(def (reduce f i xs)
  (if (nil? xs)
    i
    (reduce f (f i (car xs)) (cdr xs))))

(def (reverse l)
  (if (nil? l)
    '()
    (append (reverse (rest l)) (list (first l)))))
(def (map f l)
  (if (not (nil? l))
    (cons (f (first l))
          (map f (rest l)))))

(def (zip l1 l2)
  (if (not (or (nil? l1) (nil? l2)))
    (cons (list (first l1) (first l2))
          (zip (rest l1) (rest l2)))))

;; run a function on each element in a list
;; and return nil (ignoring the result of the function
(def (each l f)
  (if (not (nil? l))
    (do
      (f (first l))
      (each (rest l) f))))

;; take any value and repeat it n times in a list
(def (repeat val n)
  (if (zero? n)
    nil
    (cons val (repeat val (dec n)))))




;; define the condition syntax.
(defmacro (cond :rest cs)
  (if (nil? (first cs)) :no-case
    `(if ,(first (first cs)) (do ,@(rest (first cs)))
       (cond ,@(rest cs)))))


