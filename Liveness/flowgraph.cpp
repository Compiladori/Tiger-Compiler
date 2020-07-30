#include "flowgraph.h"

using namespace flowgraph;
using namespace std;

int Node::total_num = 0;
void FlowGraph::addJumps(Node *t) {
    auto i = t->_info;
    auto oper = dynamic_cast<assem::Oper *>(i);
    if ( !oper )
        return;

    auto temp_list = oper->jumps;
    for ( auto p : temp_list ) {
        auto neighbour = label_map.find(p);
        if ( neighbour != label_map.end() ) {
            if ( !_flow_graph.hasDirectEdge(t, neighbour->second) ) {
                _flow_graph.addDirectedEdge(t, neighbour->second);
            }
        } else {    //can't find label
            exit(-1);
        }
    }
}

FlowGraph::FlowGraph(util::GenericList<assem::Instruction> instruction_list) {
    auto i = instruction_list.begin();
    auto prev = make_unique<Node>((*i).get());
    auto prev_ptr = prev.get();
    if ( auto label_inst = dynamic_cast<assem::Label *>((*i).get()) ) {
        label_map[label_inst->label] = prev_ptr;
    }
    node_list.push_back(move(prev));
    i++;
    for ( ; i != instruction_list.end(); i++ ) {
        auto curr = make_unique<Node>((*i).get());
        auto curr_ptr = curr.get();
        node_list.push_back(move(curr));
        _flow_graph.addDirectedEdge(prev_ptr, curr_ptr);
        if ( auto label_inst = dynamic_cast<assem::Label *>((*i).get()) ) {
            label_map[label_inst->label] = curr_ptr;
        }
        prev_ptr = curr_ptr;
    }
    for ( const auto &node : node_list )
        addJumps(node.get());
    _flow_graph.show_graph();
}