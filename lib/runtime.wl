


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



(defmacro (fn* args :rest body)
  ,(fn ,args (do ,@body)))


(defmacro (stdproc id bname)
  `(def ,id (proc-binding "/usr/local/lib/wulf/stdbind.so" ,bname)))


(load "test.wl")
(load "string.wl")


;; --------------- MATH ----------------

(stdproc + "add")

(stdproc - "sub")
(stdproc * "mul")
(stdproc / "divide")
(stdproc mod "mod")



(stdproc pow "wulf_pow")
(stdproc sqrt "wulf_sqrt")
(stdproc cos "wulf_cos")
(stdproc sin "wulf_sin")
(stdproc tan "wulf_tan")

(def (abs x) (if (> x 0) x (- 0 x)))

(stdproc = "wulf_equal")
(stdproc < "wulf_lessthan")
(stdproc > "wulf_greaterthan")
(def (>= a b) (not (< a b)))
(def (<= a b) (not (> a b)))


(def pi 3.14159265359)

(def (signum x)
  (if (= x 0) x (/ x (abs x))))


;; --------------- LIST BINDINGS -----------------

(stdproc cons "cons")
(stdproc car "car")
(stdproc first "car")
(stdproc cdr "cdr")
(stdproc rest "cdr")


(def (second x) (car (cdr x)))

;; -----------------------------------------------

(stdproc setf "setf")


(def (nil? x) (= x nil))
(def (true? x) (not (= x nil)))
(def (zero? x) (= x 0))

(def (ident? n) (= (type n) :ident))
(def (number? n) (= (type n) :number))
(def (list? n) (= (type n) :list))
(def (string? n) (= (type n) :string))
(def (procedure? n) (= (type n) :procedure))
(def (keyword? n) (= (type n) :keyword))

;; -------------------------------------

(def (eval stmt) (syscall 2 stmt))
(def (apply f a) (eval (cons f a)))

;; -------------------------------------

(def (id x) x)

;; -------------------------------------

(def (let-helper/names args) (map car args))
(def (let-helper/vals args) (map (fn x (first (rest x))) args))

(defmacro (let vals :rest body)
  `((fn ,(let-helper/names vals)
      (do ,@body))
    ,@(let-helper/vals vals)))

(defmacro (test vals) `(,(let-helper/vals vals)))

;; -------------------------------------

(def (every f lst)
  (if (not (f (car lst)))
    nil
    (every f (cdr lst))))


(def (list-of-lists? x) (every list? x))
;; define the condition syntax.
(defmacro (cond :rest cs)
  (if (nil? (first cs)) :no-case
    `(if ,(first (first cs)) (do ,@(rest (first cs)))
       (cond ,@(rest cs)))))


;; -------------------------------------


;; add the scheme define syntax
(defmacro (define :rest args) `(def ,@args))
(defmacro (lambda :rest args) `(fn ,@args))
(defmacro (fn* args :rest body)
  `(fn ,args (do ,@body)))

(defmacro (defn name args :rest body)
  `(def ,name (fn ,args (do ,@body))))



(stdproc puts "wulf_puts")
;; put mappings to printing systemcalls
(def (print x) (do (puts x) (puts "\n")))
(def (prins :rest args)
  (do
    (each args (fn a (do (puts a) (puts " "))))
    (puts "\n")
    nil))

(def (list :rest l) l)

(def (type x) (syscall 9 x))

(def (inc a) (+ a 1))
(def (dec a) (- a 1))

;; define the "truth" value
(def t 't)


;; find the length of any list
(def (len l)
  (if (nil? l)
    0
    (+ 1 (len (cdr l)))))



;; create a list of numbers a inclusive to b exclusive



(def (range-step a b step)
  (if (< a b) (cons a (range-step (+ a step) b step))))

(def (range a b)
  (if (< a b) (cons a (range-step (inc a) b 1))))
(def (zero-to a) (range-step 0 (inc a) 1))
(def (zero-upto a) (range-step 0 a 1))





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
    (cons (list (car l1) (car l2))
          (zip (cdr l1) (cdr l2)))))

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



(def (nth/list n l)
  (if (< n 0) nil
  (if (zero? n)
    (first l)
    (nth/list (dec n) (rest l)))))

(def (nth/string n s) (strseq s n (inc n)))

(def (nth n l)
  (let ((typ (type l)))
    (cond ((= typ :list) (nth/list n l))
          ((= typ :string) (nth/string n l)))))



(stdproc make-custom "make_custom")
(stdproc malloc "wulf_malloc")

(stdproc eval-thread "eval_thread")
(stdproc join "thread_join")
(defmacro (thread expr) `(eval-thread ',expr))





(def (for/names vals) (map car vals))
(def (for/vals vals) (map second vals))
(defmacro (for vals :rest body)
  (do
    (print (for/names vals))
    (print (for/vals vals))))
