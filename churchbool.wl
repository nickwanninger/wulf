(def (true a b) a)
(def (false a b) b)

(def (NOT b)
  (b false true))

(def (AND a b) (a b false))
(def (OR a b) (a b false))
(def (xor a b) (a (b false true) b))

