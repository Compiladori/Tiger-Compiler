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

struct Liveness {
    std::vector<std::set<temp::Temp>> in, out, def ,use;
    Liveness(flowgraph::FlowGraph &flow_graph);
    void GenerateLiveInfo(flowgraph::FlowGraph &flow_graph);
};

};    // namespace liveness
#endif
