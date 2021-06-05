#ifndef __cmd_H__
#define __cmd_H__
#include <memory>
#include <set>
#include <string>

namespace cmd {

class Handler {
   protected:
    static std::shared_ptr<Handler> singleton_;

    std::set<std::string> args;

   public:
    Handler() = default;
    Handler(Handler& other) = delete;

    void operator=(const Handler&) = delete;

    static std::shared_ptr<Handler> GetInstance();

    void process_args(int argc, char** argv) {
        for ( int i = 1; i < argc; ++i ) {
            args.insert(argv[i]);
        }
    }
    bool is_option(std::string opt) {
        return args.count(opt);
    }
};
};    // namespace cmd

#endif
