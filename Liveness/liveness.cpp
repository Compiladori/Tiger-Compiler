#include "liveness.h"

using namespace liveness;
using namespace std;

#include <algorithm>    // std::set_union, std::sort
#include <iostream>     // std::cout

template <typename T>
std::set<T> getUnion(const std::set<T>& a, const std::set<T>& b) {
    std::set<T> result = a;
    result.insert(b.begin(), b.end());
    return result;
}

template <typename T>
std::set<T> getDiff(const std::set<T>& a, const std::set<T>& b) {
    std::set<T> result;
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(result, result.end()));
    return result;
}

// expects a filled set
template <typename T>
T getFirstElement(const std::set<T>& a) {
    if ( !a.size() ) exit(-1);
    return *a.begin();
}

int TempNode::total_num = 0;
void Liveness::GenerateLiveInfo(flowgraph::FlowGraph& flow_graph) {
    // in[n] = use[n] U (out[n] - def[n])
    //  out[n] = U in[s] {s, s->succ[n]}
    // TODO agarrar la lista de los nodos
    set<temp::Temp> _in, _out;
    const auto& node_list = flow_graph.node_list;
    for ( auto i = node_list.begin(); i != node_list.end(); ++i ) {
        set<temp::Temp> temp1;
        in.push_back(temp1);
        out.push_back(temp1);
        use.push_back((*i)->get_use());
        def.push_back((*i)->get_def());
    }

    bool done = false;
    while ( !done ) {
        done = true;
        for ( int i = 0; i < node_list.size(); ++i ) {
            _in = in[i];
            _out = out[i];
            //    in[n] = use[n] U (out[n] - def[n])
            //    equation 1
            in[i] = getUnion(use[i], getDiff(out[i], def[i]));
            // out[n] = U in[s] {s, s->succ[n]}
            // equation 2
            set<temp::Temp> temp_in;
            for ( auto p : flow_graph._flow_graph.getSuccessors(i) ) {
                temp_in = getUnion(temp_in, in[p]);
            }
            out[i] = temp_in;
            if ( in[i] != _in || out[i] != _out ) done = false;
        }
    }

    return;
}


void Liveness::initItfGraph(const flowgraph::NodeList& node_list) {
    std::set<temp::Temp> temps;
    for ( auto i = node_list.begin(); i != node_list.end(); ++i ) {
        temps = getUnion(temps, getUnion((*i)->get_def(), (*i)->get_use()));
    }
    for ( auto i = temps.begin(); i != temps.end(); ++i ) {
        auto node = TempNode(*i);
        _interference_graph.addNode(node);
        temp_to_node[*i] = node;
    }
}

void Liveness::InferenceGraph(flowgraph::FlowGraph& flow_graph) {
    const auto& node_list = flow_graph.node_list;
    initItfGraph(node_list);
    set<temp::Temp> defs, srcs;
    int flowgraph_node_index=0;
    for ( auto i = node_list.begin(); i != node_list.end(); ++i ) {
        defs = (*i)->get_def();
        // si es move agrega una para cada live out que no coincida con el que lo definio
        // sino para cada elemento de los defs agrega una arista con cada out
        if ( dynamic_cast<assem::Move*>((*i)->_info) ) {
            srcs = (*i)->get_use();
            auto dst = temp_to_node[getFirstElement(defs)];
            auto src = temp_to_node[getFirstElement(srcs)];
            moves.push_back(Move(dst, src));
            auto liveouts = out[flowgraph_node_index];
            for ( auto t = liveouts.begin(); t != liveouts.end(); ++t ) {
                auto t_node = temp_to_node[*t];
                if ( t_node == dst ) continue;
                // duplicar linea
                _interference_graph.addUndirectedEdge(dst, t_node);
            }
        } else {
            for ( auto def = defs.begin(); def != defs.end(); ++def ) {
                auto liveouts = out[flowgraph_node_index];
                auto dst = temp_to_node[*def];
                for ( auto t = liveouts.begin(); t != liveouts.end(); ++t ) {
                    auto src = temp_to_node[*t];
                    if ( *t == *def ) continue;
                    _interference_graph.addUndirectedEdge(dst, src);
                }
            }
        }
        flowgraph_node_index++;
    }
}

Liveness::Liveness(flowgraph::FlowGraph& flow_graph) {
    GenerateLiveInfo(flow_graph);
    InferenceGraph(flow_graph);
}
