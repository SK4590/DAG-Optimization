# Test Case 1: Standard Common Subexpression Elimination (from README)
# Demonstrates redundant expression identification and reuse
t1 = a * b
t2 = a * b
t3 = c + d
t4 = t3 - t1
t5 = c + d
t6 = t5 - t2
