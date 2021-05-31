#ifndef __FLOW_GRAPH_H__
#define __FLOW_GRAPH_H__
#include <set>
#include <unordered_map>
#include <vector>

#include "../AST/AST.h"
#include "../Frame/temp.h"
#include "../Munch/assem.h"
#include "../Utility/utility.h"
#include "graph.h"

namespace flowgraph {
class Node;
class Graph;
using NodeList = std::deque<std::shared_ptr<Node>>;

struct Node {
    std::shared_ptr<assem::Instruction> _info;
    static int total_num;
    int key;
    Node(std::shared_ptr<assem::Instruction> info) : _info(info), key(total_num++) {}
    Node() : key(total_num++) {}
    std::set<temp::Temp> get_use();
    std::set<temp::Temp> get_def();
    void print() { _info->print(); };
    bool operator==(const Node& s) const { return key == s.key; }
};

struct NodeHasher {
    std::size_t operator()(const Node s) const {
        return std::hash<int>()(s.key);
    }
};

struct FlowGraph {
    std::unordered_map<temp::Label, Node, ast::SymbolHasher> label_map;
    graph::Graph<Node, NodeHasher> _flow_graph;
    NodeList node_list;
    FlowGraph(assem::InstructionList& instruction_list);

   private:
    void addJumps(std::shared_ptr<Node> t);
};

};    // namespace flowgraph
#endif