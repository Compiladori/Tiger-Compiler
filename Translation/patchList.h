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

  void applyPatch(temp::Label label) {
    for (auto it = label_ptrs.begin(); it != label_ptrs.end(); ++it)
      **it = label;
  }

  std::size_t size() const { return label_ptrs.size(); }

  void push_back(temp::Label* label) { label_ptrs.push_back(label); }
  void pop_back() { label_ptrs.pop_back(); }
};

#endif
