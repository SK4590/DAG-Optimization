# Test Case 5: Realistic Basic Block (Matrix Index Offset Calculation)
# Typical TAC from array access: A[i][j] = B[i][j] + C[i][j]
# Notice how row offset (i * 40) and column offset (j * 4) are repeatedly computed
t1 = i * 40
t2 = j * 4
t3 = t1 + t2
t4 = B + t3
t5 = i * 40
t6 = j * 4
t7 = t5 + t6
t8 = C + t7
t9 = t4 + t8
t10 = i * 40
t11 = j * 4
t12 = t10 + t11
A = t12
