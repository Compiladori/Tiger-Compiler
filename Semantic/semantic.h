#ifndef __SEMANTIC_H__
#define __SEMANTIC_H__

/**
 * Generation of intermediate code and semantic check
 * 
 * Described in Chapter 5 Appel C (2004)
 * **/

#include <cassert>
#include <memory>
#include <vector>
#include <algorithm>
#include "../AST/AST.h"
#include "../Translation/translation.h"
#include "environmentTable.h"
#include "expressionType.h"
namespace seman {

/** 
 * Table entries
 * **/
struct TypeEntry {
  std::shared_ptr<ExpType> type;

  TypeEntry(std::shared_ptr<ExpType> type) : type(type) {}

  void print() const {}
};

struct ValueEntry {
  virtual ~ValueEntry() {}
  virtual void print() const = 0;
};

struct VarEntry : public ValueEntry {
  std::shared_ptr<ExpType> type;
  std::shared_ptr<trans::Access> access;

  VarEntry(std::shared_ptr<ExpType> type, std::shared_ptr<trans::Access> access) : type(type), access(access) {}

  void print() const {}
};

struct FunEntry : public ValueEntry {
  std::vector<std::shared_ptr<ExpType>> formals;
  std::shared_ptr<ExpType> result;
  std::shared_ptr<trans::Level> funlvl;
  std::shared_ptr<temp::Label> label;

  FunEntry(std::shared_ptr<ExpType> result, std::shared_ptr<temp::Label> label) : formals(), result(result), label(label) {}
  FunEntry(std::vector<std::shared_ptr<ExpType>> &formals, std::shared_ptr<ExpType> result, std::shared_ptr<trans::Level> lvl, std::shared_ptr<temp::Label> label) : formals(formals), result(result), funlvl(lvl), label(label) {}
  FunEntry(std::vector<std::shared_ptr<ExpType>> &&formals, std::shared_ptr<ExpType> result, std::shared_ptr<trans::Level> lvl, std::shared_ptr<temp::Label> label) : formals(formals), result(result), funlvl(lvl), label(label) {}

  void print() const {}
};

/**
 * Main semantic checking class
 * **/
class SemanticChecker {
  BindingTable<TypeEntry> TypeEnv;
  BindingTable<ValueEntry> ValueEnv;

  std::stack<std::stack<ast::Symbol>> type_insertions, value_insertions;
  std::stack<temp::Label> break_insertions;
  std::unique_ptr<trans::Translator> translator;
  std::vector<std::string> lib_fun;
  bool isFromLib(std::string item) {return std::find(lib_fun.begin(), lib_fun.end(), item) != lib_fun.end();}
  auto getTypeEntry(const ast::Symbol &s) { return TypeEnv.getEntry(s); }
  auto getValueEntry(const ast::Symbol &s) { return ValueEnv.getEntry(s); }
  auto getBreakEntry() { return break_insertions.top(); }

  void insertTypeEntry(ast::Symbol s, std::unique_ptr<TypeEntry> type_entry, bool ignore_scope = false);
  void insertValueEntry(ast::Symbol s, std::unique_ptr<ValueEntry> value_entry, bool ignore_scope = false);

  void beginScope();
  void endScope();

  void beginBreakScope(const temp::Label &s) { break_insertions.push(s); }
  void endBreakScope();
  bool BreakScopeEmpty() { return break_insertions.empty(); }

  void load_initial_values(std::shared_ptr<trans::Level> outermost);

  void clear(std::shared_ptr<trans::Level> outermost) {
    TypeEnv.clear(), ValueEnv.clear();
    type_insertions = std::stack<std::stack<ast::Symbol>>();
    value_insertions = std::stack<std::stack<ast::Symbol>>();
    break_insertions = std::stack<temp::Label>();

    load_initial_values(outermost);
  }

  seman::AssociatedExpType transVariable(std::shared_ptr<trans::Level> lvl, ast::Variable *var);
  seman::AssociatedExpType transExpression(std::shared_ptr<trans::Level> lvl, ast::Expression *exp);
  std::unique_ptr<trans::TranslatedExp> transDeclarations(std::shared_ptr<trans::Level> lvl, ast::DeclarationList *dec_list);
  std::shared_ptr<ExpType> transType(std::shared_ptr<trans::Level> lvl, ast::Type *type);

 public:
  SemanticChecker() { translator = std::make_unique<trans::Translator>(); }

  std::unique_ptr<frame::FragList> translate(ast::Expression *exp);
};

};  // namespace seman

#endif
