# DAG-Based Optimization for Basic Blocks
## 📖 Overview

Unoptimized Three-Address Code (TAC) often contains redundant computations that waste CPU cycles and memory bandwidth. This optimizer implements a full DAG-based architecture combined with an O(1) Value Numbering search algorithm to efficiently optimize basic blocks into shorter, valid TAC without duplicate computations.

**Naive Input vs Optimized Output:**
| 🛑 Unoptimized (Before) | ⚡ Optimized (After) |
| :--- | :--- |
| `t1 = a * b` | `t1 = a * b` |
| `t2 = a * b` | `t2 = t1` |
| `t3 = c + d` | `t3 = c + d` |
| `t4 = t3 - t1` | `t4 = t3 - t1` |
| `t5 = c + d` | `t5 = t3` |
| `t6 = t5 - t2` | `t6 = t4` |

## ✨ Features & Optimizations

* **Common Subexpression Elimination:** Drops the recalculation of an expression if the exact operands and operator were already processed in the block.


* **Redundant Leaf Prevention:** Reuses existing variable nodes rather than allocating new memory for duplicate operands.


* **O(1) Value Numbering:** Utilizes a Signature Hash Map strategy to generate string signatures for constant-time redundancy checks.


* **Robust File I/O:** Parses raw TAC text files dynamically and outputs the finalized code to a clean text file.


* **Graph Traversal & Code Generation:** Walks the finalized DAG structure to generate and print the new optimized Three-Address Code.



## 🏗️ Pipeline Architecture

The system runs through a complete optimization pipeline:

1. **Token Parser:** Reads the input TAC text file.


2. **DAG Constructor:** Applies Value Numbering to build the graph and link redundant variables to existing memory nodes.


3. **Code Generator:** Traverses the DAG to formulate shortened instructions.


4. **Output:** Generates the optimized TAC file.



## 🛠️ Implementation Details

* **Data Structures:** The `DAGNode` struct and critical hash maps (`valueNumbering`, `varToNode`) efficiently manage the graph relationships and memory.


* **Environment:** Written cleanly in C++ using standard libraries.


* **Performance:** Evaluates instruction counts before and after optimization to empirically prove efficiency gains.
