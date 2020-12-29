#ifndef __REGALLOC_H__
#define __REGALLOC_H__

#include <map>
#include <unordered_map>
#include <stack>
#include <unordered_set>
#include <vector>
#include "../Liveness/flowgraph.h"
#include "../Liveness/liveness.h"
#include "../Frame/frame.h"

namespace regalloc {

using namespace std; 
using InstructionList = util::GenericList<assem::Instruction>;
class Result;

struct result {
  temp::TempMap coloring;
  InstructionList instruction_list;
};

template<typename T>
bool isIn(T node, vector<T> list){
  for (auto it = list.begin(); it != list.end(); it++)
    if (*it == node) return true; 
  return false;
}

// check this!!!!!!!!!!!
//#define K 6  // F_registers().size() ??

class RegAllocator {
  liveness::Liveness live_graph;  // struct Live_graph { G_graph graph; Live_moveList moves; };
  vector<liveness::TempNode> freezeWorklist; // low-degree move-related nodes
  vector<liveness::TempNode> simplifyWorklist; // list of low-degree non-move-related nodes
  vector<liveness::TempNode> spillWorklist; // high-degree nodes
  vector<liveness::TempNode> spilledNodes; // nodes marked for spilling during this round; initially empty
  vector<liveness::TempNode> coalescedNodes; // registers that have been coalesced; when u<-v is coalesced,
  // v is added to this set and u put back on some work-list (or vice versa)
  vector<liveness::TempNode> precolored;
  int K; // regs.size()
  vector<liveness::TempNode> coloredNodes; // nodes successfully colored
  vector<liveness::TempNode> selectStack; // stack containing temporaries removed from the graph

  vector<liveness::Move> coalescedMoves; // moves that have been coalesced
  vector<liveness::Move> constrainedMoves; // moves whose source and target interfere
  vector<liveness::Move> frozenMoves; // moves that will no longer be considered for coalescing
  vector<liveness::Move> worklistMoves; // moves enabled for possible coalescing
  vector<liveness::Move> activeMoves; // moves not yet ready for coalescing
  // adjSet -> no lo defino porque uso G_nodeList G_adj(G_node n)
  // adjList -> no lo defino porque uso void G_addEdge(G_node from, G_node to)
  unordered_map<liveness::TempNode, int, liveness::TempNodeHasher> degree; // current degree of each node
  // mapping from a node to the list of moves it is associated with
  unordered_map<liveness::TempNode, vector<liveness::Move>, liveness::TempNodeHasher> moveList; 
  // en vez de definir moveList uso moveNodes que devuelve una lista ??????????
  // when a move (u,v) has been coalesced, and v put in coalescedNodes, alias(v)=u
  unordered_map<liveness::TempNode, liveness::TempNode, liveness::TempNodeHasher> alias; 
  unordered_map<liveness::TempNode, int, liveness::TempNodeHasher> nodeColors; 
  void addEdge(liveness::TempNode u, liveness::TempNode v);
  vector<liveness::TempNode> adjacent(liveness::TempNode n); 
  unordered_map<liveness::TempNode, vector<liveness::TempNode>, liveness::TempNodeHasher> adjacentNodes;
  vector<liveness::Move> nodeMoves(liveness::TempNode n);
  bool isMoveRelated(liveness::TempNode node);
  void decrementDegree(liveness::TempNode m);
  void enableMoves(vector<liveness::TempNode> nodes);
  
  void addWorklist(liveness::TempNode node);
  bool forAllAdjOk(liveness::TempNode u, liveness::TempNode v);
  bool conservative(vector<liveness::TempNode> nodes1, vector<liveness::TempNode> nodes2);
  liveness::TempNode getAlias(liveness::TempNode node); 
  void combine(liveness::TempNode u, liveness::TempNode v);
  void freezeMoves(liveness::TempNode u);
  float spillHeuristic(liveness::TempNode node);
  void clearLists();

  void build(temp::TempList regs);
  void makeWorklist();
  void simplify();
  void coalesce();
  void freeze();
  void selectSpill();
  temp::TempMap assignColors(temp::TempMap initial);
  InstructionList rewriteProgram(frame::Frame f, InstructionList instruction_list);
 public:
  RegAllocator() = default;
  result regAllocate(frame::Frame f, InstructionList instruction_list, temp::TempMap initial, temp::TempList regs);
};


};

#endif