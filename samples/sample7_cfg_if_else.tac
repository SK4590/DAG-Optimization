# Test Case 7: CFG If-Else Diamond with Conditional Branch & Fallthrough
# Demonstrates branching, convergence at merge block, and local DAG CSE
t1 = a + b
t2 = a + b
if x > 0 goto THEN_BLOCK
t3 = c * d
t4 = c * d
ans = t3 + t4
goto MERGE
THEN_BLOCK:
t5 = e - f
t6 = e - f
ans = t5 + t6
MERGE:
res = ans + t1
