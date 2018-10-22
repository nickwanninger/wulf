(defun fact (x)
  (if (zerop x)
      1
      (* x (fact (- x 1)))))
