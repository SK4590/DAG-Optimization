# Multi-Block Benchmark 6: Loop Sum Accumulation with Redundancies
# Computes: sum = sum + 4*i + 4*i in a while loop
# Demonstrates while loop CFG, common subexpression elimination, and DCE
sum = 0
i = 1
N = 100
tdead0 = 50 * 2
LOOP_HEADER:
if i > N goto LOOP_EXIT
t1 = i * 4
t2 = i * 4
t3 = sum + t1
t4 = sum + t2
tdead1 = t1 + 99
sum = t4
i = i + 1
goto LOOP_HEADER
LOOP_EXIT:
result = sum
