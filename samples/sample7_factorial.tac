# Multi-Block Benchmark 7: Factorial Computation with Conditional Branching
# Computes factorial: fact = fact * i iteratively
# Demonstrates loop control, local expression reuse, and conditional exits
n = 6
fact = 1
i = 1
FACT_LOOP:
if i > n goto FACT_END
t1 = fact * i
t2 = fact * i
tdead = t1 + 10
fact = t2
i = i + 1
goto FACT_LOOP
FACT_END:
result = fact
