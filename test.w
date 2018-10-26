; the language only provides nand...
(defun not (n) (nand n n))
(defun and (a b) (nand (nand a b) (nand a b)))

(defun or (p q)
  (nand (nand p p) (nand q q)))

(defun xor (a b)
  (nand (nand a (nand a b))
        (nand b (nand a b))))

(defun <= (a b) (not (> a b)))

(set 'f ())
(repl)
