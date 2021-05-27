#include "flowgraph.h"

#include <algorithm>    // std::set_union, std::sort
#include <iostream>     // std::cout
#include <set>
#include <vector>    // std::vec

#include "../Utility/error.h"

using namespace flowgraph;
using namespace std;

int Node::total_num = 0;
void FlowGraph::addJumps(std::shared_ptr<Node> t) {
    auto i = t->_info;
    auto oper = dynamic_cast<assem::Oper *>(i.get());
    if ( !oper )
        return;

    auto temp_list = oper->jumps;
    for ( auto p : temp_list ) {
        auto neighbour = label_map.find(p);
        if ( neighbour != label_map.end() ) {
            if ( !_flow_graph.hasDirectEdge(*t, neighbour->second) ) {
                _flow_graph.addDirectedEdge(*t, neighbour->second);
            }
        } else {    //can't find label
            throw error::internal_error("can't find label " + p.name, __FILE__);
        }
    }
}

FlowGraph::FlowGraph(assem::InstructionList &instruction_list) {
    auto i = instruction_list.begin();
    auto prev = make_shared<Node>(*i);
    if ( auto label_inst = dynamic_cast<assem::Label *>((*i).get()) ) {
        label_map[label_inst->label] = *prev;
    }
    node_list.push_back(prev);
    i++;
    for ( ; i != instruction_list.end(); i++ ) {
        auto curr = make_shared<Node>(*i);
        node_list.push_back(curr);
        _flow_graph.addDirectedEdge(*prev, *curr);
        if ( auto label_inst = dynamic_cast<assem::Label *>((*i).get()) ) {
            label_map[label_inst->label] = *curr;
        }
        prev = curr;
    }
    for ( const auto &node : node_list )
        addJumps(node);
}

std::set<temp::Temp> convertToSet(temp::TempList v) {
    std::set<temp::Temp> s;
    for ( auto x : v ) s.insert(x);
    return s;
}

std::set<temp::Temp> Node::get_use() {
    return convertToSet(_info->get_src());
}

std::set<temp::Temp> Node::get_def() {
    return convertToSet(_info->get_dst());
}