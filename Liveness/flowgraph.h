#ifndef __FLOW_GRAPH_H__
#define __FLOW_GRAPH_H__
#include <unordered_map>
#include <vector>
#include <set>

#include "../AST/AST.h"
#include "../Frame/temp.h"
#include "../Munch/assem.h"
#include "../Utility/utility.h"
#include "graph.h"

namespace flowgraph {
class Node;
class Graph;
using NodeList = util::GenericList<Node>;

struct Node {
    static int total_num;
    int key;
    assem::Instruction* _info;
    Node(assem::Instruction* info) : _info(info), key(total_num++) {}
    std::set<temp::Temp> get_use();
    std::set<temp::Temp> get_def();
    bool operator==(const Node& s) const { return key == s.key; }
};

struct NodeHasher {
    std::size_t operator()(const Node* s) const {
        return std::hash<int>()((*s).key);
    }
};

struct FlowGraph {
    std::unordered_map<temp::Label, Node*, ast::SymbolHasher> label_map;
    graph::Graph<Node*, NodeHasher> _flow_graph;
    NodeList node_list;
    FlowGraph(util::GenericList<assem::Instruction> instruction_list);

   private:
    void addJumps(Node* t);
};

};    // namespace flowgraph
#endif