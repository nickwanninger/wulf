(def zero (fn f (fn x (x))))
(def succ (fn n (fn f (fn x (f ((n f) x))))))
(def one (succ zero))

(def add (fn m (fn n ((m succ) n))))
(def two (succ one))
(def three (succ two))
(def five ((add two) three))

(def false (fn a (fn b (a))))
(def true (fn a (fn b (b))))

(def pair (fn a (fn b (fn (f) ((f a) b)))))
(def fst (fn p (p false)))
(def snd (fn p (p true)))

(def _pc0 ((pair zero) zero))
(def _f (fn p ((pair (snd p)) (succ (snd p)))))