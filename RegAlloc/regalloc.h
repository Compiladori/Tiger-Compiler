#ifndef __REGALLOC_H__
#define __REGALLOC_H__

#include <map>
#include <unordered_map>
#include <stack>
#include <unordered_set>
#include <vector>
#include "../Liveness/flowgraph.h"
#include "../Frame/frame.h"

namespace regalloc {

using InstructionList = util::GenericList<assem::Instruction>;
using NodeList = std::vector<std::shared_ptr<flowgraph::Node>>;
class Result;

struct Result {
  std::map<temp::Temp, std::string> coloring;
  InstructionList instruction_list;
};

// preguntar si estos son realmente todos los registros
// si es así, ver de hacer una función en frame que me los devuelva y hago size()
int get_K(){ 
  temp::Label lab; 
  std::vector<bool> v = std::vector<bool>();
  frame::Frame f = frame::Frame(lab, v); 
  return f.get_arg_regs().size() + f.get_caller_saved_regs().size() + f.get_callee_saved_regs().size();
}
#define K get_K()

class RegAllocator {
  // VER COMO SE LLAMA EN MODULO LIVE !!!!!!!!!!!!!!!!!!
  // Live_graph interference_graph;  // struct Live_graph { G_graph graph; Live_moveList moves; };
  flowgraph::NodeList simplifyWorklist; // list of low-degree non-move-related nodes
  flowgraph::NodeList freezeWorklist; // low-degree move-related nodes
  flowgraph::NodeList spillWorklist; // high-degree nodes
  flowgraph::NodeList spilledNodes; // nodes marked for spilling during this round; initially empty
  flowgraph::NodeList coalescedNodes; // registers that have been coalesced; when u<-v is coalesced,
  // v is added to this set and u put back on some work-list (or vice versa)
  flowgraph::NodeList coloredNodes; // nodes successfully colored
  flowgraph::NodeList selectStack; // stack containing temporaries removed from the graph
  // uso push_front y pop_front \

  //  VER COMO SE LLAMA LA MOVELIST EN MODULO FLOWGRAPH !!!!!!!!!!!!!!!!!!
  // flowgraph::moveList coalescedMoves; // moves that have been coalesced
  // flowgraph::moveList constrainedMoves; // moves whose source and target interfere
  // flowgraph::moveList frozenMoves; // moves that will no longer be considered for coalescing
  // flowgraph::moveList worklistMoves; // moves enabled for possible coalescing
  // flowgraph::moveList activeMoves; // moves not yet ready for coalescing
  // adjSet -> no lo defino porque uso G_nodeList G_adj(G_node n)
  // adjList -> no lo defino porque uso void G_addEdge(G_node from, G_node to)
  std::unordered_map<int, int> degree; // current degree of each node, <key,degree>
  // std::unordered_map<flowgraph::Node, flowgraph::moveList> moveList; // mapping from a node to the list of moves it is associated with
  // en vez de definir moveList uso moveNodes que devuelve una lista ??????????
  std::unordered_map<int, int> alias; // when a move (u,v) has been coalesced, and v put in coalescedNodes, alias(v)=u
  std::unordered_map<flowgraph::Node, int> color; 
  void addEdge(flowgraph::Node u, flowgraph::Node v);
  // flowgraph::NodeList adjacent(flowgraph::Node* n); 
  std::unordered_map<int, flowgraph::NodeList*> adjacentNodes;
  bool isMoveRelated(flowgraph::Node* node);
  // en vez de definir moveRelated escribo directo NodeMoves(n) != {} donde sea necesario
  void decrementDegree(flowgraph::Node* m);
  void enableMoves(flowgraph::NodeList* nodes);
  void addWorklist(flowgraph::Node* node);
  bool OK(flowgraph::Node t, flowgraph::Node r);
  bool conservative(flowgraph::NodeList nodes);
  flowgraph::Node getAlias(flowgraph::Node node); 
  void combine(flowgraph::Node* u, flowgraph::Node* v);
  void freezeMoves(flowgraph::Node u);

  void build();
  void makeWorklist();
  void simplify();
  void coalesce();
  void freeze();
  void selectSpill();
  void assignColors();
  void rewriteProgram(frame::Frame f, InstructionList instruction_list);
 public:
  RegAllocator() = default;
  Result regAllocate(frame::Frame f, InstructionList instruction_list);
};




};

#endif
