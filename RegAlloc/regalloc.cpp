#include "regalloc.h"

using namespace regalloc;

void addEdge(flowgraph::Node u, flowgraph::Node v){}

bool RegAllocator::isMoveRelated(flowgraph::Node* node){
  // for (auto it = worklistMoves.begin(); it != worklistMoves.end(); it++)
  //   if (it->get()->src == node || it->get()->dst == node)
  //     return true;

  // for (auto it = activeMoves.begin(); it != activeMoves.end(); it++)
  //   if (it->get()->src == node || it->get()->dst == node)
  //     return true;
  // return false;
}

void RegAllocator::decrementDegree(flowgraph::Node* m){
  int d = degree[m->key];
  degree[m->key] = d-1;
  if (d == K){
    flowgraph::NodeList* adj = adjacentNodes[m->key];
    adj->push_front(m);
    enableMoves(adj);
    auto it = spillWorklist.begin();
    for(; it->get() != m; it++);
    spillWorklist.erase(it);

    if (isMoveRelated(m))
      freezeWorklist.push_back(m);
    else 
      simplifyWorklist.push_back(m);
  }
}

void RegAllocator::enableMoves(flowgraph::NodeList* nodes){
  // for (auto it1 = nodes->begin(); it1 != nodes->end(); it1++){
  //   for (auto it2 = activeMoves.begin(); it2 != activeMoves.end(); it2++){
  //     if (it2->get()->src == it1->get() || it2->get()->dst == it1->get()){
  //       activeMoves.erase(it2);
  //       worklistMoves.push_back(it2->get());
  //     }
  //   }
  // }
}

void addWorklist(flowgraph::Node* node){}

bool OK(flowgraph::Node t, flowgraph::Node r){}

bool RegAllocator::conservative(flowgraph::NodeList nodes){
  // checkear 
  int k = 0;
  for (auto it = nodes.begin(); it != nodes.end(); it++)
    if (degree[it->get()->key] >= K) k++;
  return (k < K);
}

flowgraph::Node RegAllocator::getAlias(flowgraph::Node node){}

void combine(flowgraph::Node* u, flowgraph::Node* v){}

void RegAllocator::freezeMoves(flowgraph::Node u){}

void RegAllocator::build(){
  // VER COMO SE LLAMA EN MODULO LIVEGRAPH
  // nodes = g_nodes(interference_graph.graph) funcion g_nodes devuelve los nodos del grafo
  // while(!nodes.empty()){
  //    node = nodes->pop_front();
  //    degree.insert({ node.get()->key, g_degree(node)}); funcion g_degree devuelve grado del nodo
  //    adjacentNodes.insert({ *node.get(), G_adj(node)}); ver tipo de G_adj
  // }
  // FALTA INICIALIZAR COLOR, tal vez no es necesario
}

void RegAllocator::makeWorklist(){
  // classify nodes into sets
  // VER COMO SE LLAMA EN MODULO GRAPH
  // G_nodeList nodes = interference_graph.graph.G_nodes();
  // for(auto it = nodes->begin(); it != nodes->end(); it++){ //ver si se puede recorrer así
  //     int degree = degree[it->get()->key];
  //     if (degree >= K) spillWorklist.push_back(it->get()); //spillWorklist : NodeList
  //     else if (isMoveRelated(it->get())) freezeWorklist.push_back(it->get());
  //     else simplifyWorklist.push_back(it->get);
  // }
  // worklistMoves = interference_graph.moves; // Live_graph { G_graph graph; Live_moveList moves; }
}

void RegAllocator::simplify(){
  auto n = simplifyWorklist.pop_front();
  selectStack.push_front(n.get()); 
  for (auto it = adjacentNodes[n.get()->key]->begin(); it != adjacentNodes[n.get()->key]->end(); it++)
    decrementDegree(it->get());
}

void coalesce(){}

void RegAllocator::freeze(){
  auto u = freezeWorklist.pop_front(); // freezeWorklist : NodeList
  freezeMoves(*u.get()); // cambié el orden
  simplifyWorklist.push_back(std::move(u));
}

void RegAllocator::selectSpill(){
  // heuristic
}

void RegAllocator::assignColors(){}

void RegAllocator::rewriteProgram(frame::Frame f, InstructionList instruction_list){}

Result RegAllocator::regAllocate(frame::Frame f, InstructionList instruction_list){
  do {
      // VER COMO SE LLAMA EN FLOWGRAPH !!!!!!!!!!!!!!!!!!!!!!!!
      // graph::Graph graph = flowgraph::assemFlowGraph(instruction_list); 
      // interference_graph = liveness(graph);
      build();
      makeWorklist();
      do {
          if (!simplifyWorklist.empty()) simplify();
          //else if (!worklistMoves.empty()) coalesce();
          else if (!freezeWorklist.empty()) freeze();
          else if (!spillWorklist.empty()) selectSpill();
      } while (!simplifyWorklist.empty() //or !worklistMoves.empty()
          or !freezeWorklist.empty() or !spillWorklist.empty());
      assignColors();
      if (!spilledNodes.empty())
          rewriteProgram(f, std::move(instruction_list)); // spilledNodes no hace falta que se lo pase
  } while (!spilledNodes.empty());
}
