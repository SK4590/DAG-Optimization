# Test Case 4: Constant Folding & Algebraic Simplifications
# Evaluates compile-time constants and eliminates neutral/annihilating operands
t1 = 12 + 8
t2 = t1 * 2
t3 = a * 1
t4 = b + 0
t5 = c * 0
t6 = d - d
t7 = t2 + 5
