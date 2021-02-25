#ifndef __LIVENESS_H__
#define __LIVENESS_H__

/**
 * Generation of inference graph
 * 
 * Described in Chapter 10 Appel C (2004)
 * **/

#include <set>
#include <vector>

#include "../Frame/temp.h"
#include "../Munch/assem.h"
#include "../Utility/utility.h"
#include "flowgraph.h"

namespace liveness {
class TempNode;

struct TempNode {
    static int total_num;
    int key;
    temp::Temp _info;
    TempNode(const TempNode &node) : _info(node._info), key(node.key) {}
    TempNode(temp::Temp info) : _info(info), key(total_num++) {}
    TempNode& operator=(const TempNode &node) {_info = node._info; key = node.key; return *this;}
    bool operator==(const TempNode &s) const { return _info == s._info; }
    TempNode() = default;
};

struct TempNodeHasher {
    std::size_t operator()(const TempNode &s) const {
        return std::hash<int>()(s._info.num);
    }
};

struct Move {
    TempNode dst;
    TempNode src;
    Move(TempNode dst, TempNode src) : dst(dst), src(src) {}
};

struct Liveness {
    std::vector<std::set<temp::Temp>> in, out, def, use;
    graph::Graph<TempNode, TempNodeHasher> _interference_graph;
    std::unordered_map<temp::Temp, TempNode, TempNodeHasher> temp_to_node;
    std::vector<Move> moves;
    Liveness(flowgraph::FlowGraph &flow_graph);
    Liveness() = default;
    void GenerateLiveInfo(flowgraph::FlowGraph &flow_graph);
    void InferenceGraph(flowgraph::FlowGraph &flow_graph);
    void initItfGraph(const flowgraph::NodeList &node_list);
};

};    // namespace liveness
#endif
