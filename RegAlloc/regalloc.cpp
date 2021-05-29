#include "regalloc.h"

using namespace regalloc;
using namespace std;

void RegAllocator::addEdge(liveness::TempNode u, liveness::TempNode v) {
    if ( !isIn(u, adjList[v]) and !(u == v) ) {
        if ( !isIn(u._info, precolored) ) {
            adjList[u].push_back(v);
            degree[u] = degree.at(u) + 1;
        }
        if ( !isIn(v._info, precolored) ) {
            adjList[v].push_back(u);
            degree[v] = degree.at(v) + 1;
        }
    }
}
// function Adjacent(n) adjList[n] \ (selectStack ∪ coalescedNodes)
// we use adjlist for precolored also but avoid cuestioning
vector<liveness::TempNode> RegAllocator::adjacent(liveness::TempNode n) {
    if ( isIn(n._info, precolored) ) {
        return vector<liveness::TempNode>();
    }
    auto adjacentNodes = adjList.at(n);
    vector<liveness::TempNode> except = selectStack;
    for ( auto it = coalescedNodes.begin(); it != coalescedNodes.end(); it++ )
        except.push_back(*it);
    for ( auto it1 = adjacentNodes.begin(); it1 != adjacentNodes.end(); ) {
        auto it2 = find(except.begin(), except.end(), *it1);
        if ( it2 != except.end() ) {
            it1 = adjacentNodes.erase(it1);

        } else {
            it1++;
        }
    }
    return adjacentNodes;
}

vector<liveness::Move> RegAllocator::nodeMoves(liveness::TempNode n) {
    vector<liveness::Move> unionAW = activeMoves;    // union activeMoves and worklistMoves
    for ( auto it = worklistMoves.begin(); it != worklistMoves.end(); it++ )
        unionAW.push_back(*it);

    vector<liveness::Move> moveListN = moveList.at(n), result;
    for ( auto it1 = moveListN.begin(); it1 != moveListN.end(); it1++ ) {
        for ( auto it2 = unionAW.begin(); it2 != unionAW.end(); it2++ )
            if ( it1->dst == it2->dst and it1->src == it2->src )
                result.push_back(*it1);
    }
    return result;
}

bool RegAllocator::isMoveRelated(liveness::TempNode node) {
    vector<liveness::Move> unionWA = worklistMoves;
    for ( auto it = activeMoves.begin(); it != activeMoves.end(); it++ )
        unionWA.push_back(*it);
    if ( !moveList.count(node) ) {
        return false;
    }
    vector<liveness::Move> moveListN = moveList.at(node);
    for ( auto it1 = moveListN.begin(); it1 != moveListN.end(); it1++ ) {
        for ( auto it2 = unionWA.begin(); it2 != unionWA.end(); it2++ ) {
            if ( it1->src == it2->src and it1->dst == it2->dst ) {
                // it1 == it2 en la interseccion de moveList y la union worklistMoves y activeMoves
                return true;
            }
        }
    }
    return false;
}

void RegAllocator::decrementDegree(liveness::TempNode m) {
    auto d = degree.at(m);
    degree[m] = d - 1;
    if ( d == K && !isIn(m._info, precolored) ) {
        vector<liveness::TempNode> adj = adjacent(m);
        adj.push_back(m);
        enableMoves(adj);
        auto it = find(spillWorklist.begin(), spillWorklist.end(), m);
        if ( it != spillWorklist.end() ) {
            spillWorklist.erase(it);
        }

        if ( isMoveRelated(m) )
            freezeWorklist.push_back(m);
        else
            simplifyWorklist.push_back(m);
    }
}

void RegAllocator::enableMoves(vector<liveness::TempNode> nodes) {
    bool flag;
    for ( auto node_it = nodes.begin(); node_it != nodes.end(); node_it++ ) {
        for ( auto act_mov_it = activeMoves.begin(); act_mov_it != activeMoves.end(); ) {
            flag = true;
            if ( moveList.count(*node_it) ) {
                auto move_list = moveList.at(*node_it);
                for ( auto it3 = move_list.begin(); it3 != move_list.end(); it3++ ) {
                    if ( (*it3) == (*act_mov_it) ) {
                        worklistMoves.push_back(*act_mov_it);
                        act_mov_it = activeMoves.erase(act_mov_it);
                        flag = false;
                    }
                }
            }
            if ( flag ) {
                act_mov_it++;
            }
        }
    }
}

// function NodeMoves (n) moveList[n] ∩ (activeMoves ∪ worklistMoves)  include moveList[n] ∩ activeMoves
// procedure EnableMoves(nodes)
// forall n ∈ nodes
//     forall m ∈ NodeMoves (n)
//     if m ∈ activeMoves then
//         activeMoves ← activeMoves \ {m}
//         worklistMoves ← worklistMoves ∪ {m}

// procedure AddWorkList(u)
// if (u ̸∈ precolored ∧ not(MoveRelated(u)) ∧ degree[u] < K) then
//  freezeWorklist ← freezeWorklist \ {u}
//  simplifyWorklist ← simplifyWorklist ∪ {u}

void RegAllocator::addWorklist(liveness::TempNode node) {
    if ( !isIn(node._info, precolored) and !isMoveRelated(node) && (degree.at(node) < K) ) {
        auto it = find(freezeWorklist.begin(), freezeWorklist.end(), node);
        if ( it != freezeWorklist.end() ) {
            freezeWorklist.erase(it);
        }
        simplifyWorklist.push_back(node);
    }
}

bool RegAllocator::forAllAdjOk(liveness::TempNode u, liveness::TempNode v) {
    vector<liveness::TempNode> adjs = adjacent(v);
    for ( auto it = adjs.begin(); it != adjs.end(); it++ ) {
        if ( !((degree.at(*it) < K) or isIn((*it)._info, precolored) or isIn(*it, adjList.at(u))) )
            return false;
    }
    return true;
}

bool RegAllocator::conservative(vector<liveness::TempNode> nodes1, vector<liveness::TempNode> nodes2) {
    vector<liveness::TempNode> nodes = nodes1;
    for ( auto n : nodes2 ) nodes.push_back(n);
    int k = 0;
    for ( auto it = nodes.begin(); it != nodes.end(); it++ )
        if ( degree.at(*it) >= K ) k++;
    return (k < K);
}

liveness::TempNode RegAllocator::getAlias(liveness::TempNode node) {
    if ( isIn(node, coalescedNodes) )
        return getAlias(alias.at(node));
    return node;
}

void RegAllocator::combine(liveness::TempNode u, liveness::TempNode v) {
    auto it = find(freezeWorklist.begin(), freezeWorklist.end(), v);
    if ( it != freezeWorklist.end() ) {
        freezeWorklist.erase(it);
    } else {
        auto it = find(spillWorklist.begin(), spillWorklist.end(), v);
        if ( it != spillWorklist.end() ) {
            spillWorklist.erase(it);
        }
    }
    coalescedNodes.push_back(v);
    alias[v] = u;
    for ( auto it = moveList.at(v).begin(); it != moveList.at(v).end(); it++ ) {
        moveList.at(u).push_back(*it);
    }
    auto adjacentNodes = adjacent(v);
    for ( auto it = adjacentNodes.begin(); it != adjacentNodes.end(); it++ ) {
        addEdge(*it, u);
        decrementDegree(*it);
    }
    it = find(freezeWorklist.begin(), freezeWorklist.end(), u);
    if ( degree.at(u) >= K and it != freezeWorklist.end() ) {
        freezeWorklist.erase(it);
        spillWorklist.push_back(u);
    }
}

void RegAllocator::freezeMoves(liveness::TempNode u) {
    vector<liveness::Move> moves = nodeMoves(u);
    liveness::TempNode v;
    for ( auto it = moves.begin(); it != moves.end(); it++ ) {
        if ( getAlias(it->dst) == getAlias(u) )
            v = getAlias(it->src);
        else
            v = getAlias(it->dst);
        auto idx = activeMoves.begin();
        for ( ; idx != activeMoves.end(); idx++ )
            if ( it->dst == idx->dst and it->src == idx->src )
                break;
        activeMoves.erase(idx);
        frozenMoves.push_back(*it);
        if ( nodeMoves(v).empty() and degree.at(v) < K ) {
            auto idx = find(freezeWorklist.begin(), freezeWorklist.end(), v);
            if ( idx != freezeWorklist.end() ) {
                freezeWorklist.erase(idx);
            }
            freezeWorklist.erase(idx);
            simplifyWorklist.push_back(v);
        }
    }
}

float RegAllocator::spillHeuristic(liveness::TempNode node) {
    // heuristic: (uses + defs) / degree
    int idx = live_graph._interference_graph.keyToId(node);
    int uses_and_defs = live_graph.use[idx].size() + live_graph.def[idx].size();    // live_graph.use[idx] : std::vector<std::set<temp::Temp>>
    float value = uses_and_defs * 1.0 / degree.at(node);
    return value;
}

// procedure Build ()
// forall b ∈ blocks in program
// let live = liveOut(b)
// forall I ∈ instructions(b) in reverse order
// if isMoveInstruction(I) then
// live ← live\use(I)
// forall n ∈ def(I) ∪ use(I)
// moveList[n] ← moveList[n] ∪ {I}
// worklistMoves ← worklistMoves ∪ {I}
// live ← live ∪ def(I)
// forall d ∈ def(I)
// forall l ∈ live
// AddEdge(l, d)
// live ← use(I) ∪ (live\def(I))

void RegAllocator::build(assem::InstructionList instruction_list) {
    flowgraph::FlowGraph graph = flowgraph::FlowGraph(instruction_list);
    graph._flow_graph.show_graph();
    live_graph = liveness::Liveness(graph);
    live_graph._interference_graph.show_graph();
    adjList = live_graph._interference_graph.getAdjacentList();
    worklistMoves = live_graph.workListmoves;
    moveList = live_graph.moveList;
    auto nodes = live_graph._interference_graph.getNodes();
    for ( auto node : nodes ) {
        if ( isIn(node._info, precolored) ) {
            degree[node] = 99999;
        } else {
            degree[node] = live_graph._interference_graph.getDegree(node);
            initial.push_back(node);
        }
    }
}

// procedure MakeWorklist()
//     forall n ∈ initial
//     initial ← initial \ { n }
// if degree
//     [n] ≥ K then
//         spillWorklist ← spillWorklist ∪ { n }
// else if MoveRelated (n)
//     then
//         freezeWorklist ← freezeWorklist ∪ { n }
// else
//     simplifyWorklist ← simplifyWorklist ∪ { n }

void RegAllocator::makeWorklist() {
    for ( auto it = initial.begin(); it != initial.end(); it++ ) {
        int deg = degree.at(*it);
        if ( deg >= K )
            spillWorklist.push_back(*it);
        else if ( isMoveRelated(*it) )
            freezeWorklist.push_back(*it);
        else
            simplifyWorklist.push_back(*it);
    }
}

void RegAllocator::simplify() {    // remove non-move-related nodes of low (< K ) degree from the graph
    auto n = simplifyWorklist.back();
    simplifyWorklist.pop_back();
    selectStack.push_back(n);
    vector<liveness::TempNode> adjs = adjacent(n);
    for ( auto it = adjs.begin(); it != adjs.end(); it++ )
        decrementDegree(*it);
}

void RegAllocator::coalesce() {
    auto n = worklistMoves.back();
    worklistMoves.pop_back();
    liveness::TempNode x = getAlias(n.src);
    liveness::TempNode y = getAlias(n.dst);
    liveness::TempNode u = x, v = y;    //liveness::TempNode u, v;
    if ( isIn(y._info, precolored) ) {
        u = y;
        v = x;
    }
    if ( u == v ) {
        coalescedMoves.push_back(n);
        addWorklist(u);
    } else if ( isIn(v._info, precolored) or isIn(u, adjList[v]) ) {
        constrainedMoves.push_back(n);
        addWorklist(u);
        addWorklist(v);
    } else if ( (isIn(u._info, precolored) and forAllAdjOk(u, v)) or
                (!isIn(u._info, precolored) and conservative(adjacent(u), adjacent(v))) ) {
        coalescedMoves.push_back(n);
        combine(u, v);
        addWorklist(u);
    } else
        activeMoves.push_back(n);
}

// u ∈ precolored ∧ (∀t ∈ Adjacent(v), OK(t, u))
// ∨ u ̸∈ precolored ∧
// Conservative(Adjacent(u) ∪ Adjacent(v))

// If neither simplify nor coalesce applies, we look for a move-related node of low degree.
// We freeze the moves in which this node is involved: that is, we give up hope of coalescing
// those moves. This causes the node (and perhaps other nodes related to the frozen moves) to
// be considered non-move-related, which should enable more simplification.
void RegAllocator::freeze() {
    auto u = freezeWorklist.back();
    freezeWorklist.pop_back();
    simplifyWorklist.push_back(u);
    freezeMoves(u);
}

void RegAllocator::selectSpill() {    // heuristic
    auto idx = spillWorklist.begin();
    float value, min_value = -1;
    for ( auto it = spillWorklist.begin(); it != spillWorklist.end(); it++ ) {
        value = spillHeuristic(*it);
        if ( value < min_value ) {
            min_value = value;
            idx = it;
        }
    }
    auto tmp = *idx;
    spillWorklist.erase(idx);
    simplifyWorklist.push_back(tmp);
    freezeMoves(tmp);
}

temp::TempMap RegAllocator::assignColors() {
    while ( !selectStack.empty() ) {
        auto ok_colors = avail_colors;    // todos los colores disponibles
        liveness::TempNode n = selectStack.back();
        selectStack.pop_back();
        // if ( coloring.find(n._info) != coloring.end() ) continue;
        vector<liveness::TempNode> nodes = adjList[n];
        for ( auto it = nodes.begin(); it != nodes.end(); it++ ) {
            auto alias_node = getAlias(*it);
            auto alias_temp = alias_node._info;
            if ( isIn(alias_node, coloredNodes) or isIn(alias_temp, precolored) ) {
                auto color = coloring.at(alias_temp);
                auto toErease = find(ok_colors.begin(), ok_colors.end(), color);
                if ( toErease != ok_colors.end() ) {
                    ok_colors.erase(toErease);
                }
            }
        }
        if ( !ok_colors.empty() ) {
            auto color = ok_colors.front();
            coloring[n._info] = color;
            std::cout << "assigning color: " + color.name + " to :" << n._info.num << std::endl;
            coloredNodes.push_back(n);
        } else {
            spilledNodes.push_back(n);
        }
    }
    std::cout << "coalesced Coloring" << std::endl;
    for ( auto it = coalescedNodes.begin(); it != coalescedNodes.end(); it++ ) {
        if ( isIn(getAlias(*it), spilledNodes) )
            spilledNodes.push_back(*it);
        else {
            auto alias_color = coloring.find(getAlias(*it)._info);
            if ( alias_color != coloring.end() ) {
                std::cout << "assigning color: " + alias_color->second.name + " to :" << (*it)._info.num << std::endl;
                coloring[(*it)._info] = alias_color->second;
            } else {
                throw error::internal_error("coalesced node not finded in coloring", __FILE__);
            }
        }
    }
    return coloring;
}
// procedure RewriteProgram()
// Allocate memory locations for each v ∈ spilledNodes,
// Create a new temporary vi for each definition and each use,
// In the program (instructions), insert a store after each
// definition of a vi , a fetch before each use of a vi .
// Put all the vi into a set newTemps.
// spilledNodes ← {}
// initial ← coloredNodes ∪ coalescedNodes ∪ newTemps
// coloredNodes ← {}
// coalescedNodes ← {}
assem::InstructionList RegAllocator::rewriteProgram(frame::Frame f, assem::InstructionList instruction_list) {
    for ( auto it1 = spilledNodes.begin(); it1 != spilledNodes.end(); it1++ ) {
        std::cout << "Spilling temp: " << it1->_info.num << std::endl;
        assem::InstructionList new_instruction_list;
        shared_ptr<frame::Access> mem = f.alloc_local(true);
        int offset = dynamic_cast<frame::InFrame*>(mem.get())->offset;
        for ( auto it2 = instruction_list.begin(); it2 != instruction_list.end(); it2++ ) {
            bool flag_dst = false;
            if ( auto i = dynamic_cast<assem::Oper*>(it2->get()) ) {
                temp::Temp t = temp::Temp();
                temp::TempList new_src;
                for ( auto it = i->src.begin(); it != i->src.end(); it++ ) {
                    if ( *it == it1->_info ) {
                        new_src.push_back(t);
                        new_instruction_list.push_back(make_shared<assem::Oper>("movq " + to_string(offset) + "(%'s0), %'d0 #spilled", temp::TempList{frame::Frame::fp_temp()}, temp::TempList{t}, temp::LabelList{}));
                    } else
                        new_src.push_back(*it);
                }
                i->src = new_src;
                temp::TempList new_dst;
                for ( auto it = i->dst.begin(); it != i->dst.end(); it++ ) {
                    if ( *it == it1->_info ) {
                        new_dst.push_back(t);
                        flag_dst = true;
                    } else
                        new_dst.push_back(*it);
                }
                i->dst = new_dst;
                new_instruction_list.push_back(*it2);
                if ( flag_dst ) {
                    new_instruction_list.push_back(make_shared<assem::Oper>("movq %'s1, " + to_string(offset) + "(%'s0) #spilled", temp::TempList{frame::Frame::fp_temp(), t}, temp::TempList{}, temp::LabelList{}));
                }
            } else if ( auto i = dynamic_cast<assem::Move*>(it2->get()) ) {
                temp::Temp t = temp::Temp();
                temp::TempList new_src;
                for ( auto it = i->src.begin(); it != i->src.end(); it++ ) {
                    if ( *it == it1->_info ) {
                        new_src.push_back(t);
                        new_instruction_list.push_back(make_shared<assem::Oper>("movq " + to_string(offset) + "(%'s0), %'d0 #spilled", temp::TempList{frame::Frame::fp_temp()}, temp::TempList{t}, temp::LabelList{}));
                    } else
                        new_src.push_back(*it);
                }
                i->src = new_src;
                temp::TempList new_dst;
                for ( auto it = i->dst.begin(); it != i->dst.end(); it++ ) {
                    if ( *it == it1->_info ) {
                        new_dst.push_back(t);
                        flag_dst = true;
                    } else
                        new_dst.push_back(*it);
                }
                i->dst = new_dst;
                new_instruction_list.push_back(*it2);
                if ( flag_dst ) {
                    new_instruction_list.push_back(make_shared<assem::Oper>("movq %'s1, " + to_string(offset) + "(%'s0) #spilled", temp::TempList{frame::Frame::fp_temp(), t}, temp::TempList{}, temp::LabelList{}));
                }
            } else {
                new_instruction_list.push_back(*it2);
            }
        }
        instruction_list = new_instruction_list;
    }
    // spilledNodes.clear();
    // coloredNodes.clear();
    // coalescedNodes.clear();
    return instruction_list;
}

void RegAllocator::clearLists() {
    freezeWorklist.clear();
    simplifyWorklist.clear();
    spillWorklist.clear();
    coalescedNodes.clear();
    coloredNodes.clear();
    selectStack.clear();
    coalescedMoves.clear();
    constrainedMoves.clear();
    frozenMoves.clear();
    worklistMoves.clear();
    activeMoves.clear();
    degree.clear();
    moveList.clear();
    alias.clear();
    nodeColors.clear();

    adjList.clear();
}

result RegAllocator::main(frame::Frame f, assem::InstructionList instruction_list) {
    avail_colors.clear();
    precolored.clear();
    temp::TempMap initial_coloring;
    temp::TempMap temp_to_reg_map = f.get_temp_to_reg_map();
    auto reg_map = f.get_reg_to_temp_map();
    auto reg_list = f.get_rets();
    for ( auto it = reg_list.begin(); it != reg_list.end(); it++ )
        precolored.push_back(reg_map[*it]);
    K = precolored.size();
    for ( auto it = precolored.begin(); it != precolored.end(); it++ ) {
        auto lbl = temp::Label(temp_to_reg_map[*it]);
        initial_coloring[*it] = lbl;
        avail_colors.push_back(lbl);
    }
    coloring = initial_coloring;
    clearLists();
    result res;
    build(instruction_list);
    makeWorklist();
    do {
        if ( !simplifyWorklist.empty() )
            simplify();
        else if ( !worklistMoves.empty() )
            coalesce();
        else if ( !freezeWorklist.empty() )
            freeze();
        else if ( !spillWorklist.empty() )
            selectSpill();
    } while ( !simplifyWorklist.empty() or !worklistMoves.empty() or !freezeWorklist.empty() or !spillWorklist.empty() );
    assignColors();
    if ( !spilledNodes.empty() ) {
        instruction_list = rewriteProgram(f, instruction_list);
        return main(f, instruction_list);
    }
    res.instruction_list = instruction_list;
    res.coloring = coloring;
    return res;
}
result RegAllocator::regAllocate(frame::Frame f, assem::InstructionList instruction_list) {
    result res;
    do {
        res = main(f, instruction_list);
    } while ( !spilledNodes.empty() );
    return res;
}