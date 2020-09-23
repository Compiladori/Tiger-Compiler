#include "regalloc.h"

using namespace regalloc;
using namespace std;

void RegAllocator::addEdge(liveness::TempNode u, liveness::TempNode v){
  if (!isIn(u, adjacentNodes[v]) and !(u == v)){
    if (!isIn(u, precolored)){
      adjacentNodes[u].push_back(v);
      degree[u] = degree[u] + 1;
    }
    if (!isIn(v, precolored)){
      adjacentNodes[v].push_back(u);
      degree[v] = degree[v] + 1;
    }
  }

}

vector<liveness::TempNode> RegAllocator::adjacent(liveness::TempNode n){
  vector<liveness::TempNode> adjList = adjacentNodes[n];
  vector<liveness::TempNode> except = selectStack;
  for (auto it = coalescedNodes.begin(); it != coalescedNodes.end(); it++)
    except.push_back(*it);
  for (auto it1 = adjList.begin(); it1 != adjList.end(); it1++){
    auto it2 = find(except.begin(), except.end(), *it1);
    if (it2 != except.end())  
      adjList.erase(it1);    
  }
  return adjList;
}

vector<liveness::Move> RegAllocator::nodeMoves(liveness::TempNode n){
  vector<liveness::Move> unionAW = activeMoves; // union activeMoves and worklistMoves
  for (auto it = worklistMoves.begin(); it != worklistMoves.end(); it++)
    unionAW.push_back(*it);
  
  vector<liveness::Move> moveListN = moveList[n], result;
  for (auto it1 = moveListN.begin(); it1 != moveListN.end(); it1++){
    for (auto it2 = unionAW.begin(); it2 != unionAW.end(); it2++)
      if (it1->dst == it2->dst and it1->src == it2->src)
        result.push_back(*it1);
  }
  return result;
}

bool RegAllocator::isMoveRelated(liveness::TempNode node){
  for (auto it = worklistMoves.begin(); it != worklistMoves.end(); it++)
    if (it->src == node || it->dst == node) 
      return true;

  for (auto it = activeMoves.begin(); it != activeMoves.end(); it++)
    if (it->src == node || it->dst == node)
      return true;
  return false;
}

void RegAllocator::decrementDegree(liveness::TempNode m){
  auto d = degree[m];
  degree[m] = d-1;
  if (d == K && !isIn(m, precolored)){
    vector<liveness::TempNode> adj = adjacent(m);
    adj.push_back(m);
    enableMoves(adj);
    auto it = find(spillWorklist.begin(), spillWorklist.end(), m);
    spillWorklist.erase(it);

    if (isMoveRelated(m))
      freezeWorklist.push_back(m);
    else 
      simplifyWorklist.push_back(m);
  }
}

void RegAllocator::enableMoves(vector<liveness::TempNode> nodes){
  for (auto it1 = nodes.begin(); it1 != nodes.end(); it1++){
    for (auto it2 = activeMoves.begin(); it2 != activeMoves.end(); it2++){
      if (it2->src == *it1 || it2->dst == *it1){ 
        activeMoves.erase(it2);
        worklistMoves.push_back(*it2);
      }
    }
  }
}

bool RegAllocator::isIn(liveness::TempNode node, vector<liveness::TempNode> list){
  for (auto it = list.begin(); it != list.end(); it++)
    if (*it == node) return true; 
  return false;
}

void RegAllocator::addWorklist(liveness::TempNode node){
  if (!isIn(node, precolored) and !isMoveRelated(node) && (degree[node] < K)){
    auto it = find(freezeWorklist.begin(), freezeWorklist.end(), node);
    freezeWorklist.erase(it);
    simplifyWorklist.push_back(node);
  }
}

bool RegAllocator::forAllAdjOk(liveness::TempNode u, liveness::TempNode v){
  vector<liveness::TempNode> adjs = adjacent(v);
  for (auto it = adjs.begin(); it != adjs.end(); it++){
    if (! ((degree[*it] < K) or isIn(*it, precolored) or isIn(*it, adjacentNodes[u]) ))
      return false;
  }
  return true;
}

bool RegAllocator::conservative(vector<liveness::TempNode> nodes1, vector<liveness::TempNode> nodes2){
  vector<liveness::TempNode> nodes = nodes1;
  for (auto n : nodes2) nodes.push_back(n);
  int k = 0;
  for (auto it = nodes.begin(); it != nodes.end(); it++)
    if (degree[*it] >= K) k++;
  return (k < K);
}

liveness::TempNode RegAllocator::getAlias(liveness::TempNode node){
  if (isIn(node, coalescedNodes))
    return getAlias(alias[node]);
  return node;
}

void RegAllocator::combine(liveness::TempNode u, liveness::TempNode v){
  if (isIn(v, freezeWorklist)){
    auto it = find(freezeWorklist.begin(), freezeWorklist.end(), v);
    freezeWorklist.erase(it);
  } else {
    auto it = find(spillWorklist.begin(), spillWorklist.end(), v);
    spillWorklist.erase(it);
  }
  coalescedNodes.push_back(v);
  alias[v] = u;
  for (auto it = moveList[v].begin(); it != moveList[v].end(); it++)
    moveList[u].push_back(*it);
  
  for (auto it = adjacent(v).begin(); it != adjacent(v).end(); it++){
    addEdge(*it, v);
    decrementDegree(*it);
  }
  if (degree[u] >= K and isIn(u, freezeWorklist)){
    auto it = find(freezeWorklist.begin(), freezeWorklist.end(), u);
    freezeWorklist.erase(it);
    spillWorklist.push_back(u);
  }

}

void RegAllocator::freezeMoves(liveness::TempNode u){  
  vector<liveness::Move> moves = nodeMoves(u);
  liveness::TempNode v;
  for (auto it = moves.begin(); it != moves.end(); it++){
    if (getAlias(it->dst) == getAlias(u))
      v = getAlias(it->src);
    else 
      v = getAlias(it->dst);
    auto idx = activeMoves.begin();
    for (; idx != activeMoves.end(); idx++)
      if (it->dst == idx->dst and it->src == idx->src)
        break;
    activeMoves.erase(idx);
    frozenMoves.push_back(*it);
    if (nodeMoves(v).empty() and degree[v] < K){
      auto idx = find(freezeWorklist.begin(), freezeWorklist.end(), v);
      freezeWorklist.erase(idx);
      simplifyWorklist.push_back(v);
    }
  }
}

float RegAllocator::spillHeuristic(liveness::TempNode node){
  // heuristic: (uses + defs) / degree
  int idx = live_graph._interference_graph.keyToId(node);
  int uses_and_defs = live_graph.use[idx].size() + live_graph.def[idx].size(); // live_graph.use[idx] : std::vector<std::set<temp::Temp>>
  float value = uses_and_defs * 1.0 / degree[node];
  return value;
}

void RegAllocator::build(frame::Frame f){
  vector<liveness::TempNode> nodes = live_graph._interference_graph.getNodes();
  while(!nodes.empty()){
    liveness::TempNode node = nodes.back();
    nodes.pop_back();
    degree.insert( {node, live_graph._interference_graph.getDegree(node)} );
    // para cambiar esto tengo que cambiar el tipo de getSuccessors
    int i = live_graph._interference_graph.keyToId(node); 
    set<int> adj = live_graph._interference_graph.getSuccessors(i);
    for (auto n : adj){
      liveness::TempNode t = live_graph._interference_graph.idToKey(n);
      adjacentNodes[node].push_back(t);
    } 
  }
  frame::RegToTempMap regToTemp = f.get_reg_to_temp_map();
  for (auto it = regToTemp.begin(); it != regToTemp.end(); it++){
    precolored.push_back(live_graph.temp_to_node[it->second]);
  }
  vector<liveness::Move> moves = live_graph.moves;
  for (auto it = moves.begin(); it != moves.end(); it++){
    moveList[(*it).src].push_back(*it);
    if (! ((*it).src == (*it).dst) )
      moveList[(*it).dst].push_back(*it);
  }
}

void RegAllocator::makeWorklist(){
  vector<liveness::TempNode> nodes = live_graph._interference_graph.getNodes();
  for (auto it = nodes.begin(); it != nodes.end(); it++){
    int deg = degree[*it];
    if (deg >= K) spillWorklist.push_back(*it);
    else if (isMoveRelated(*it)) freezeWorklist.push_back(*it);
    else simplifyWorklist.push_back(*it);
  }
  worklistMoves = live_graph.moves;
}

void RegAllocator::simplify(){ // remove non-move-related nodes of low (< K ) degree from the graph
  auto n = simplifyWorklist.back();
  simplifyWorklist.pop_back();
  selectStack.push_back(n); 
  vector<liveness::TempNode> adjs = adjacent(n);
  for (auto it = adjs.begin(); it != adjs.end(); it++)
    decrementDegree(*it);
}

void RegAllocator::coalesce(){
  auto n = worklistMoves.back();
  liveness::TempNode x = getAlias(n.src);
  liveness::TempNode y = getAlias(n.dst);
  liveness::TempNode u, v;
  if (isIn(y, precolored)){
    u = y;
    v = x;
  } else {
    u = x;
    v = y; 
  }
  worklistMoves.pop_back();
  if (u == v){
    coalescedMoves.push_back(n);
    addWorklist(u);
  } else if (isIn(v, precolored) or isIn(u, adjacentNodes[v])){
    constrainedMoves.push_back(n);
    addWorklist(u);
    addWorklist(v);
  } else if ((isIn(u, precolored) and forAllAdjOk(u, v)) or 
            (!isIn(u, precolored) and conservative(adjacent(u), adjacent(v)))){
    coalescedMoves.push_back(n);
    combine(u, v);
    addWorklist(u);
  } else 
    activeMoves.push_back(n);
}

// If neither simplify nor coalesce applies, we look for a move-related node of low degree. 
// We freeze the moves in which this node is involved: that is, we give up hope of coalescing 
// those moves. This causes the node (and perhaps other nodes related to the frozen moves) to 
// be considered non-move-related, which should enable more simplification.
void RegAllocator::freeze(){
  auto u = freezeWorklist.back();
  freezeWorklist.pop_back();
  simplifyWorklist.push_back(u);
  freezeMoves(u);
}

void RegAllocator::selectSpill(){// heuristic
  auto idx = spillWorklist.begin();
  float value, min_value = -1;
  for (auto it = spillWorklist.begin(); it != spillWorklist.end(); it++){
    value = spillHeuristic(*it);
    if (value < min_value){
      min_value = value;
      idx = it;
    }
  }
  spillWorklist.erase(idx);
  simplifyWorklist.push_back(*idx);
  freezeMoves(*idx);
}

void RegAllocator::assignColors(){
  while(!selectStack.empty()){
    liveness::TempNode n = selectStack.back();
    selectStack.pop_back();
    int ok_colors[K];
    for (int i = 0; i < K; i++) ok_colors[i] = 1; // 1 -> color disponible, 0 -> no disponible
    vector<liveness::TempNode> nodes = adjacentNodes[n];
    vector<liveness::TempNode> precolored_colored = coloredNodes;
    for (auto p : precolored) precolored_colored.push_back(p);
    for (auto it = nodes.begin(); it != nodes.end(); it++){
      if (isIn(getAlias(*it), precolored_colored)){
        int color = nodeColors[getAlias(*it)]; 
        ok_colors[color] = 0;
      }
    }
    // ok_colors == {}
    int avail_color, i;
    for (i = 0; i < K; i++) 
      if(ok_colors[i]){
        avail_color = i;
        break;
      }
    if (i == K) // ok_colors is empty
      spilledNodes.push_back(n);
    else {
      coloredNodes.push_back(n);
      nodeColors[n] = i;
    }
  }
}

void RegAllocator::rewriteProgram(frame::Frame f, InstructionList instruction_list){}

result RegAllocator::regAllocate(frame::Frame f, InstructionList instruction_list){
  do {
      // g_nodes() : vector<flowgraph::Node>, g_adj : vector<flowgraph::Node>
      // graph::Graph graph = flowgraph::assemFlowGraph(instruction_list); 
      flowgraph::FlowGraph graph = flowgraph::FlowGraph(instruction_list);
      live_graph = liveness::Liveness(graph); // moves : vector<pair<node,node>>, graph : G_graph
      build(f);
      makeWorklist();
      do {
          if (!simplifyWorklist.empty()) simplify();
          else if (!worklistMoves.empty()) coalesce();
          else if (!freezeWorklist.empty()) freeze();
          else if (!spillWorklist.empty()) selectSpill();
      } while (!simplifyWorklist.empty() or !worklistMoves.empty()
          or !freezeWorklist.empty() or !spillWorklist.empty());
      assignColors();
      if (!spilledNodes.empty())
          rewriteProgram(f, move(instruction_list)); // spilledNodes no hace falta que se lo pase
  } while (!spilledNodes.empty());
}