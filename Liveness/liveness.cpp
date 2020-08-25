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

// void get_all_variables(){
//     for ( auto i = node_list.begin(); i != node_list.end(); ++i ) {
//         temp_list = getUnion(temp_list,getUnion((*i)->get_use(),(*i)->get_def()));
//     }
//     for ( auto i = temp_list.begin(); i != temp_list.end(); ++i ) {

//     }
// }

void Liveness::InferenceGraph(flowgraph::FlowGraph& flow_graph) {
    const auto& node_list = flow_graph.node_list;
    set<temp::Temp> defs;
    int j;
    for ( auto i = node_list.begin(); i != node_list.end(); ++i) {
        if ( dynamic_cast<assem::Move*>((*i)->_info) ) {
        } else {
            defs = (*i)->get_def();
            for ( auto def = defs.begin(); def != defs.end(); ++def ) {
                auto liveouts = out[j];
                 auto dst = Node(*def);  
                for ( auto t = liveouts.begin(); t != liveouts.end(); ++t ) {
                    if ( *t == *def ) continue;
                    _interference_graph.addDirectedEdge(*def, *t);
                }
            }
        }
        j++;
        // si es  move agrega una para cada live out que no coincida con el que lo definio

        // sino para cada elemento de los defs agrega una arista con cada out
    }
}

// static G_graph
// inteferenceGraph(G_nodeList nl, G_table liveMap)
// {
//   /* init a graph node-info save temp type
//    * with create a all-regs list
//    * with ctrate a temp -> node table
//    */
//   tempMap = TAB_empty(); // g only map node to temp. need a quick
//                          // lookup for temp to node.
//   G_graph g = initItfGraph(nl, tempMap);

//   printf("inteferenceGraph:\n");
//   G_show(stdout, G_nodes(g), Temp_print); // debug
//   Temp_tempList liveouts;
//   for (; nl; nl = nl->tail) {
//     AS_instr i = (AS_instr)G_nodeInfo(nl->head);
//     assert(i);
//     Temp_tempList defs = FG_def(nl->head);

//     if (i->kind == I_MOVE) {
//       Temp_tempList srcs = FG_use(nl->head);
//       assert(defs->tail == NULL); // our move instruction only have 1 def.
//       assert(srcs->tail == NULL); // and from 1 reg.

//       G_node dst = (G_node)TAB_look(tempMap, defs->head);
//       G_node src = (G_node)TAB_look(tempMap, srcs->head);

//       MOV_addlist(&moves, src, dst); // add to movelist

//       liveouts = G_look(liveMap, nl->head);

//       for (; liveouts; liveouts = liveouts->tail) {
//         // look which node by map temp -> node
//         G_node t = (G_node)TAB_look(tempMap, liveouts->head);

//         if (dst == t) continue;

//         // TODO:
//         // We don't have to add next edge if we did coalescing.
//         // currently we skip it so I comment it out.
//         // when doing coalescing, uncomment next line.
//         if (liveouts->head == srcs->head) continue;

//         G_addEdge(dst, t);
//       }
//     }
//     else { // i->kind == I_OPER

//       printInsNode(G_nodeInfo(nl->head)); // FG

//       ////printf("in:\n");
//       ////Temp_printList(intl);
//       // printf("out:\n");
//       // Temp_printList(liveouts);
//       // printf("\n");

//       for (; defs; defs = defs->tail) {
//         //   assert(i->kind == I_OPER || i->kind == I_LABEL);
//         //   XXX:must place it here!!!
//         //   otherwise second defs will get empty
//         //   liveouts.
//         liveouts = G_look(liveMap, nl->head);

//         G_node dst = (G_node)TAB_look(tempMap, defs->head);
//         for (; liveouts; liveouts = liveouts->tail) {
//           G_node t = (G_node)TAB_look(tempMap, liveouts->head);

//           if (dst == t) continue;
//           G_addEdge(dst, t);
//           //        printf("add edge:\n");
//           //        Temp_print(liveouts->head);
//           //        Temp_print(defs->head);
//           //        printf("---\n");
//         }
//       }
//     }
//   }

//   printf("\ninteferenceGraph:\n");
//   G_show(stdout, G_nodes(g), Temp_print); // debug
//   printf("end of inteferenceGraph\n");
//   return g;
// }
