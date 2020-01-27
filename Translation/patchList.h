#ifndef __PATCH_LIST_H__
#define __PATCH_LIST_H__

#include <vector>
#include "../Frame/temp.h"

/**
 * List of pending label patches
 *
 * Explanation starting in page 155 Appel C (2004)
 * **/
struct PatchList {
    std::vector<temp::Label*> label_ptrs;

    PatchList() = default;
    PatchList(const PatchList& PL)                              : label_ptrs(PL.label_ptrs) {}
    PatchList(std::initializer_list<temp::Label*>&& label_list) : label_ptrs(label_list) {}

    void applyPatch(temp::Label label) const {
        for(auto ptr : label_ptrs){
            ptr = &label;
        }
    }

    std::size_t size() const { return label_ptrs.size(); }

    void push_back(temp::Label* label){ label_ptrs.push_back(label); }
    void pop_back(){ label_ptrs.pop_back(); }

};

#endif
