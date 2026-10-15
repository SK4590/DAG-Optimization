# Test Case 2: Commutative Common Subexpression Elimination
# In arithmetic, a + b is identical to b + a, and x * y is identical to y * x.
t1 = a + b
t2 = b + a
t3 = x * y
t4 = y * x
t5 = t1 + t3
t6 = t4 + t2
