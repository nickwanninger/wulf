import itertools


names = [
    "caar",
    "cadr",
    "cdar",
    "cddr",
    "caaar",
    "caadr",
    "cadar",
    "caddr",
    "cdaar",
    "cdadr",
    "cddar",
    "cdddr",
    "caaaar",
    "caaadr",
    "caadar",
    "caaddr",
    "cadaar",
    "cadadr",
    "caddar",
    "cadddr",
    "cdaaar",
    "cdaadr",
    "cdadar",
    "cdaddr",
    "cddaar",
    "cddadr",
    "cddddr",
    "cddadr"
]

def reverse(s):
    if len(s) == 0:
        return s
    else:
        return reverse(s[1:]) + s[0]
for name in names:
    chars = reverse(name[1:-1])
    defn = "l"
    for c in chars:
        defn = "(c" + c + "r " + defn + ")"
    defn = "(def (" + name + " l) " + defn + ")"
    print(defn)
