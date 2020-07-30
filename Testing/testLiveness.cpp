#include "../Liveness/flowgraph.h"
#include "../Munch/assem.h"
#include "catch2/catch.hpp"

using namespace std;

TEST_CASE("flowgraph", "[liveness]") {
    SECTION("TEST") {
        auto list = util::GenericList<assem::Instruction>();
        string str = "label1:";
        std::string code = "jmp `d0";
        auto lbl_1 = temp::Label();
        auto lbl_3 = temp::Label();
        auto lbl_2 = temp::Label();
        list.push_back(std::make_unique<assem::Label>(str, lbl_1));
        list.push_back(make_unique<assem::Oper>(code, temp::TempList{temp::Temp()}, temp::TempList{}, temp::LabelList({lbl_3})));
        list.push_back(std::make_unique<assem::Label>(str, lbl_2));
        list.push_back(make_unique<assem::Oper>(code, temp::TempList{temp::Temp()}, temp::TempList{}, temp::LabelList({lbl_2})));
        list.push_back(std::make_unique<assem::Label>(str, lbl_3));
        auto a = flowgraph::FlowGraph(move(list));
        REQUIRE(a.node_list.size() == 5);
    }
}
