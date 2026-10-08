#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct DAGNode {
    int id;
    string op; 
    int leftChild;
    int rightChild;
    vector<string> labels;
};

class DAGOptimizer {
private:
    vector<DAGNode> nodes;
    
    unordered_map<string, int> valueNumbering; 
   
    unordered_map<string, int> varToNode;      

    int getOrCreateLeaf(const string& varName) {
        if (varToNode.count(varName)) {
            return varToNode[varName];
        }
        int id = nodes.size();
        nodes.push_back({id, varName, -1, -1, {varName}});
        varToNode[varName] = id;
        return id;
    }

public:
    void processInstruction(const string& result, const string& arg1, const string& op, const string& arg2) {
        int leftId = getOrCreateLeaf(arg1);
        int rightId = getOrCreateLeaf(arg2);

        string signature = op + "," + to_string(leftId) + "," + to_string(rightId);
        
        if (valueNumbering.count(signature)) {
    
            int existingId = valueNumbering[signature];
            nodes[existingId].labels.push_back(result);
            varToNode[result] = existingId;
            cout << "[OPTIMIZATION] '" << result << "' mapped to existing Node " << existingId << " (Redundant Computation Eliminated)\n";
        } else {
  
            int newId = nodes.size();
            nodes.push_back({newId, op, leftId, rightId, {result}});
            valueNumbering[signature] = newId;
            varToNode[result] = newId;
        }
    }

    void printDAG() {
        cout << "\n--- Constructed DAG Nodes ---\n";
        for (const auto& node : nodes) {
            cout << "Node " << node.id << " | Op/Val: " << node.op 
                 << " | Left: " << node.leftChild << " | Right: " << node.rightChild 
                 << " | Identifiers: ";
            for (const string& lbl : node.labels) {
                cout << lbl << " ";
            }
            cout << "\n";
        }
    }
};

int main() {
    DAGOptimizer dag;
    cout << "Parsing Basic Block Instructions...\n";
    cout << "1. t1 = a + b\n2. t2 = c + d\n3. t3 = a + b\n4. t4 = t1 - t2\n\n";

    dag.processInstruction("t1", "a", "+", "b");
    dag.processInstruction("t2", "c", "+", "d");
    dag.processInstruction("t3", "a", "+", "b");
    dag.processInstruction("t4", "t1", "-", "t2");

    dag.printDAG();

    return 0;
}
