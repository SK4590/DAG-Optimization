# Test Case 8: Multi-Block Loop with Dead Temporary Copies and Transitive DCE
# Demonstrates backward fixed-point LVA and Global Dead Code Elimination
x = 10
y = 20
t1 = x * y
t2 = x * y
tdead1 = 100 + 200
START:
t3 = x + 1
t4 = x + 1
t5 = t3 * 2
tdead2 = t5 + 50
x = t5
if x < 100 goto START
ans = x + t1
