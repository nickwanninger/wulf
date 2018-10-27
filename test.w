(defun not (n) (nand n n))
(defun and (a b) (nand (nand a b) (nand a b)))
(defun or (p q)
  (nand (nand p p) (nand q q)))
(defun xor (a b)
  (nand (nand a (nand a b))
        (nand b (nand a b))))

(defun <= (a b) (not (> a b)))

(defun fib (n)
  (if (< n 2)
    1
    (+ (fib (- n 1)) (fib (- n 2)))))



(print (rest '(1 2 3 4)))
(repl)

