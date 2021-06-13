#include <sstream>

#include "../Munch/assem.h"
#include "catch2/catch.hpp"

using namespace std;
TEST_CASE("print", "[assem]") {
    // must return [label]
    temp::Label lbl = temp::Label();
    temp::Temp tmp = temp::Temp();
    temp::TempMap map;
    map[tmp] = lbl;
    SECTION("Oper") {
        stringstream out;
        auto oper = make_unique<assem::Oper>("jmp 'j0", temp::TempList{}, temp::TempList{}, temp::LabelList(1, lbl));
        oper->output(out, map);
        auto str = out.str();
        REQUIRE(str.compare("    jmp L38\n") == 0);
    }
    SECTION("Label") {
        stringstream out;
        auto oper = make_unique<assem::Label>(lbl.name + ":", lbl);
        oper->output(out, map);
        auto str = out.str();
        REQUIRE(str.compare("L40:\n") == 0);
    }

    SECTION("Move") {
        stringstream out;
        temp::Temp tmp2 = temp::Temp();
        temp::Label lbl2 = temp::Label();
        map[tmp2] = lbl2;
        auto oper = make_unique<assem::Move>("movq %'s0, %'d0", temp::TempList(1, tmp), temp::TempList(1, tmp2));
        oper->output(out, map);
        auto str = out.str();
        REQUIRE(str.compare("    movq %L42, %L44\n") == 0);
    }
}