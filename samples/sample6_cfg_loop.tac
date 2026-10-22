# Test Case 6: CFG While Loop with Conditional Branch and Loop-Back Edge
# Computes array operations and accumulator in a loop
i = 0
sum = 0
LOOP:
t1 = i * 4
t2 = base + t1
t3 = i * 4
t4 = t2 + t3
sum = sum + t4
i = i + 1
if i < 10 goto LOOP
ans = sum
