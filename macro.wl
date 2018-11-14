
(def (let-names args) (map car args))
(def (let-vals args) (map (fn x (nth 1 x)) args))

(defmacro (let vals-n-names :rest body)
  `((fn
      ,(let-names vals-n-names)
      (do ,@body))
    ; expand arg vals
    ,@(let-vals vals-n-names)))

(let ((x 3) (a 'b))
  (prins a)
  (+ x x))
