#include <set>

#include "../Liveness/flowgraph.h"
#include "../Liveness/liveness.h"
#include "../Munch/assem.h"
#include "catch2/catch.hpp"
using namespace std;

TEST_CASE("flowgraph", "[liveness]") {
    SECTION("FLOWGRAPH") {
        auto list = std::deque<std::shared_ptr<assem::Instruction>>();
        string str = "label1:";
        std::string code = "jmp `d0";
        std::string code_move = "mv `d0 0";
        std::string comp_code = "cmpq s0, s1";
        std::string jump_code = "jl 'j0";
        auto lbl_1 = temp::Label();
        auto lbl_3 = temp::Label();
        auto lbl_2 = temp::Label();
        temp::Temp a, b, c,zero,one,two;
        list.push_back(make_unique<assem::Move>(code_move, temp::TempList{zero}, temp::TempList{a}));
        list.push_back(make_unique<assem::Label>(str, lbl_1));
        list.push_back(make_unique<assem::Oper>(code_move, temp::TempList{a,one}, temp::TempList{b}, temp::LabelList{}));
        list.push_back(make_unique<assem::Oper>(code_move, temp::TempList{c, b}, temp::TempList{c}, temp::LabelList{}));
        list.push_back(make_unique<assem::Oper>(code_move, temp::TempList{two,b}, temp::TempList{a}, temp::LabelList{}));
        list.push_back(make_unique<assem::Oper>(comp_code, temp::TempList{}, temp::TempList{a}, temp::LabelList{}));
        list.push_back(make_unique<assem::Oper>(jump_code, temp::TempList{}, temp::TempList{}, temp::LabelList{lbl_1, lbl_2}));
        list.push_back(make_unique<assem::Label>(str, lbl_2));

        auto flow_graph = flowgraph::FlowGraph(list);
        REQUIRE(flow_graph.node_list.size() == 8);
        auto live = liveness::Liveness(flow_graph);
        REQUIRE(live.use[2] == set<temp::Temp>{a,one});
        REQUIRE(live.def[2] == set<temp::Temp>{b});

        REQUIRE(live.use[3] == set<temp::Temp>{b, c});
        REQUIRE(live.def[3] == set<temp::Temp>{c});


        REQUIRE(live.use[4] == set<temp::Temp>{b,two});
        REQUIRE(live.def[4] == set<temp::Temp>{a});

        REQUIRE(live.in[0] == set<temp::Temp>{c,zero,one,two});
        REQUIRE(live.in[2] == set<temp::Temp>{a, c,one,two});
        REQUIRE(live.in[3] == set<temp::Temp>{b, c,one,two});
        REQUIRE(live.in[4] == set<temp::Temp>{b, c,one,two});
        REQUIRE(live.in[5] == set<temp::Temp>{c,one,two});
        REQUIRE(live.in[6] == set<temp::Temp>{a, c,one,two});

        REQUIRE(live.out[0] == set<temp::Temp>{a, c,one,two});
        REQUIRE(live.out[2] == set<temp::Temp>{b, c,one,two});
        REQUIRE(live.out[3] == set<temp::Temp>{b, c,one,two});
        REQUIRE(live.out[4] == set<temp::Temp>{c,one,two});
        REQUIRE(live.out[5] == set<temp::Temp>{a, c,one,two});
        REQUIRE(live.out[6] == set<temp::Temp>{a, c,one,two});
        //live._interference_graph.show_graph();
    }
    SECTION("LIVENESS") {
        REQUIRE(1 == 1);
    }
}
