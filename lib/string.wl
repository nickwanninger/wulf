(stdproc str "wulf_str")
(stdproc strlen "wulf_strlen")
(stdproc strcat "wulf_strcat")
(stdproc strseq "wulf_strseq")

(def (stri s n) (strseq s n (+ n 1)))
