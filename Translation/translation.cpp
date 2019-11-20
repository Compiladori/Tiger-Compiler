#include "translation.h"

using namespace trans;

/**
 * Main translating class
 * **/
std::unique_ptr<TranslatedExp> Translator::simpleVar() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::fieldVar() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::subscriptVar() {
    // TODO: Complete
    return nullptr;
}

/**
 * Expression translation
 * **/
std::unique_ptr<TranslatedExp> Translator::varExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::unitExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::nilExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::intExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::stringExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::callExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::opExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::recordExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::seqExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::assignExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::ifExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::whileExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::forExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::letExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::breakExp() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::arrayExp() {
    // TODO: Complete
    return nullptr;
}

/**
 * Declaration translation
 * **/
std::unique_ptr<TranslatedExp> Translator::varDec() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::typeDec() {
    // TODO: Complete
    return nullptr;
}
std::unique_ptr<TranslatedExp> Translator::funDec() {
    // TODO: Complete
    return nullptr;
}

Level::Level(std::unique_ptr<Level> level,temp::Label name, std::vector<bool> list){
    _parent = std::move(level);
    _frame = std::make_unique<frame::Frame>(name,list);
}

std::unique_ptr<Access> alloc_local(std::unique_ptr<Level> level,bool escape){
    std::unique_ptr<frame::Access> access = level->_frame->alloc_local(escape);
    return std::make_unique<Access>(std::move(level),std::move(access));
}

std::unique_ptr<AccessList> formals(std::unique_ptr<Level> level) {
  std::unique_ptr<AccessList> trans_list = std::make_unique<AccessList>();
  for (auto& i : level->_frame->formals()) {
    std::unique_ptr<Access> trans_access = std::make_unique<Access>(std::move(level),std::move(i));
    trans_list->push_back(std::move(trans_access));
  }
  return trans_list;
}

static std::unique_ptr<Level> outermost_level = nullptr;

std::unique_ptr<Level> outermost(){
  if(outermost_level == nullptr){
    temp::Label tiger_label = temp::Label("mainLevel");
    outermost_level = std::make_unique<Level>(nullptr,tiger_label,std::vector<bool>());
  }
  return std::move(outermost_level);
}
