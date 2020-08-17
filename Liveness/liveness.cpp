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

Liveness::Liveness(flowgraph::FlowGraph& flow_graph) {
    GenerateLiveInfo(flow_graph);
}