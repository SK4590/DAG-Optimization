# Multi-Block Benchmark 8: Nested If-Else Branches & Dead Code
# Tests diamond CFG structure, path-based branch targets, and cross-block LVA
a = 15
b = 25
t1 = a + b
t2 = a + b
tdead_entry = 888
if a < b goto THEN_BLOCK
x = a - b
y = a * 2
goto MERGE_BLOCK
THEN_BLOCK:
x = b - a
y = b * 2
MERGE_BLOCK:
t3 = x + y
t4 = x + y
result = t3 + t1
