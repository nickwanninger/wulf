import random


def rand_op():
    return random.choice(["+", "-"])


def expr_gen(depth = 10):
    if depth == 0:
        return str(random.randrange(1.0, 10.0))
    return "(" + rand_op() + " " + expr_gen(depth-1) + " " + expr_gen(depth-1) + ")"

print("(print " + expr_gen() + ")")
