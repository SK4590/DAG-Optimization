# Test Case 3: Variable Reassignment with Node Kill
# Demonstrates proper DAG label detach and kill when a variable is reassigned
a = b + c
t1 = a - d
c = b + c
a = 10
t2 = a - d
t3 = b + c
