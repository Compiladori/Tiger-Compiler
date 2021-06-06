#include "cmd.h"
using namespace cmd;

std::shared_ptr<Handler> Handler::singleton_;

std::shared_ptr<Handler> Handler::GetInstance() {
    if ( !singleton_ ) {
        singleton_ = std::make_shared<Handler>();
    }
    return singleton_;
}