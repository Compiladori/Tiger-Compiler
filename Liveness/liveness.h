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

// TODO: Implement the Liveness module. Use either the set-equation algorithm with
// the array-of-boolean or sorted-list-of-temporaries representation of sets, or
// the one-variable-at-a-time method.

namespace liveness {

struct Node {
    int key;
    temp::Temp _info;
    Node(temp::Temp info) : _info(info) {}
    bool operator==(const Node& s) const { return _info == s._info; }
};

struct NodeHasher {
    std::size_t operator()(const Node s) const {
        return std::hash<int>()(s._info.num);
    }
};

struct Liveness {
    std::vector<std::set<temp::Temp>> in, out, def ,use;
    graph::Graph<Node,NodeHasher> _interference_graph;
    Liveness(flowgraph::FlowGraph &flow_graph);
    void GenerateLiveInfo(flowgraph::FlowGraph &flow_graph);
    void InferenceGraph(flowgraph::FlowGraph& flow_graph);
};

};    // namespace liveness
#endif
