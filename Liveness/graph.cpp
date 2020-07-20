#include "graph.h"

#include "../Frame/temp.h"
#include "../Munch/assem.h"
#include "../Utility/utility.h"
#include "flowgraph.h"
using namespace std;
int main(int argc, char const *argv[]) {
  auto list = util::GenericList<assem::Instruction>();
  string str = "label1:";
  std::string code = "jmp `d0";
  auto lbl_1 = temp::Label();
  auto lbl_3 = temp::Label();
  auto lbl_2 = temp::Label();
  list.push_back(std::make_unique<assem::Label>(str, lbl_1));
  list.push_back(make_unique<assem::Oper>(code, temp::TempList {temp::Temp()}, temp::TempList {}, assem::Targets({lbl_3})));
  list.push_back(std::make_unique<assem::Label>(str, lbl_2));
  list.push_back(make_unique<assem::Oper>(code, temp::TempList {temp::Temp()}, temp::TempList {}, assem::Targets({lbl_2})));
  list.push_back(std::make_unique<assem::Label>(str, lbl_3));
  list.push_back(make_unique<assem::Oper>(code, temp::TempList {temp::Temp()}, temp::TempList {}, assem::Targets({lbl_1})));
  list.push_back(make_unique<assem::Oper>(code, temp::TempList {temp::Temp()}, temp::TempList {}, assem::Targets({lbl_3})));
  list.push_back(make_unique<assem::Oper>(code, temp::TempList {temp::Temp()}, temp::TempList {}, assem::Targets({lbl_1})));
  auto a = flowgraph::FlowGraph(move(list));
  return 0;
}
