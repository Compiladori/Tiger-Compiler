#ifndef __ERROR_H__
#define __ERROR_H__

namespace error{

class semantic_error : public std::exception {
    std::string info;
    ast::Position pos;
    // Add (if needed) file, function

public:
        semantic_error(std::string msg, ast::Position pos) : info(msg), pos(pos) {}

        std::string getMessage() { return info + " in line " + pos.to_string(); }
    };
};

#endif